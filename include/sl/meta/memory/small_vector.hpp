//
// Created by usatiynyan.
// KISS impl.
//

#pragma once

#include "sl/meta/monad/maybe.hpp"

#include <cstddef>
#include <new>
#include <utility>

namespace sl::meta {

template <typename T, std::size_t N>
struct small_vector {
    using value_type = T;
    static constexpr std::size_t static_capacity = N;

public:
    small_vector() = default;

    small_vector(small_vector&& other) noexcept { take_from(other); }

    small_vector& operator=(small_vector&& other) noexcept {
        if (this != &other) {
            reset();
            take_from(other);
        }
        return *this;
    }

    small_vector(const small_vector&) = delete;
    small_vector& operator=(const small_vector&) = delete;

    ~small_vector() noexcept { reset(); }

public:
    bool is_static() const { return capacity_ == static_capacity; }
    std::size_t capacity() const { return capacity_; }
    std::size_t size() const { return size_; }

    template <typename... Args>
    T& emplace_back(Args&&... args) {
        const bool was_static = is_static();
        if (size_ == capacity_) [[unlikely]] { // need to grow
            const std::size_t new_capacity = capacity_ * 2;
            std::byte* prev_data = was_static ? static_data_ : data_;
            std::byte* new_data =
                static_cast<std::byte*>(::operator new(new_capacity * sizeof(T), std::align_val_t{ alignof(T) }));
            for (std::size_t i = 0; i < size_; ++i) {
                T* prev_elem = reinterpret_cast<T*>(prev_data) + i;
                T* new_elem = reinterpret_cast<T*>(new_data) + i;
                ::new (new_elem) T{ std::move(*prev_elem) };
                prev_elem->~T();
            }
            if (!was_static) {
                ::operator delete(prev_data, std::align_val_t{ alignof(T) });
            }
            data_ = new_data;
            capacity_ = new_capacity;
        }
        T* back_elem = end();
        ++size_;
        return *::new (back_elem) T{ std::forward<Args>(args)... };
    }
    void push_back(T value) { std::ignore = emplace_back(std::move(value)); }

    meta::maybe<T> pop_back() {
        if (size_ == 0) {
            return meta::null;
        }
        --size_;
        T* back_elem = end();
        meta::maybe<T> result{ std::move(*back_elem) };
        back_elem->~T();
        return result;
    }

    meta::maybe<T&> at(std::size_t i) {
        if (i >= size_) {
            return meta::null;
        }
        return reinterpret_cast<T*>(data_)[i];
    }
    meta::maybe<const T&> at(std::size_t i) const {
        if (i >= size_) {
            return meta::null;
        }
        return reinterpret_cast<const T*>(data_)[i];
    }
    T& at_unchecked(std::size_t i) { return at(i).value(); }
    const T& at_unchecked(std::size_t i) const { return at(i).value(); }

    T* begin() { return reinterpret_cast<T*>(data_); }
    const T* begin() const { return reinterpret_cast<const T*>(data_); }
    T* end() { return begin() + size_; }
    const T* end() const { return begin() + size_; }

private:
    void reset() noexcept {
        for (std::size_t i = 0; i < size_; ++i) {
            begin()[i].~T();
        }
        if (!is_static()) {
            ::operator delete(data_, std::align_val_t{ alignof(T) });
            data_ = static_data_;
            capacity_ = static_capacity;
        }
        size_ = 0;
    }

    void take_from(small_vector& other) noexcept {
        size_ = other.size_;
        capacity_ = other.capacity_;
        if (other.is_static()) {
            for (std::size_t i = 0; i < size_; ++i) {
                ::new (begin() + i) T{ std::move(other.begin()[i]) };
                other.begin()[i].~T();
            }
        } else {
            data_ = other.data_;
            other.data_ = other.static_data_;
        }
        other.capacity_ = static_capacity;
        other.size_ = 0;
    }

private:
    alignas(T) std::byte static_data_[N * sizeof(T)] = {};
    std::byte* data_ = static_data_;
    std::size_t capacity_ = static_capacity;
    std::size_t size_ = 0;
};

} // namespace sl::meta
