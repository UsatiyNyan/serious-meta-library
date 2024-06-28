//
// Created by usatiynyan.
// Courtesy of millions of examples from the internet and then some.
// non-owning intrusive forward list (usable as stack or queue)
//

#pragma once

#include "sl/meta/func/const.hpp"

#include <libassert/assert.hpp>

namespace sl::meta {

template <typename T>
struct intrusive_forward_list_node {
    T* downcast() { return static_cast<T*>(this); }
    const T* downcast() const { return static_cast<T*>(this); }

    intrusive_forward_list_node* intrusive_next = nullptr;
};

template <typename T>
class intrusive_forward_list {
public:
    using node_type = intrusive_forward_list_node<T>;
    template <bool is_const>
    class iterator;

    intrusive_forward_list() = default;

    intrusive_forward_list(const intrusive_forward_list&) = delete;
    intrusive_forward_list& operator=(const intrusive_forward_list&) = delete;

    intrusive_forward_list(intrusive_forward_list&& other) noexcept : m_{ std::exchange(other.m_, m_type{}) } {}
    intrusive_forward_list& operator=(intrusive_forward_list&& other) { m_ = std::exchange(other.m_, m_type{}); }

    void push_back(node_type* node) {
        DEBUG_ASSERT(node != nullptr);
        ++m_.size;

        node->intrusive_next = nullptr;

        if (empty()) {
            m_.head = m_.tail = node;
        } else {
            m_.tail->intrusive_next = node;
            m_.tail = node;
        }
    }

    void push_front(node_type* node) {
        DEBUG_ASSERT(node != nullptr);
        ++m_.size;

        if (empty()) {
            m_.head = m_.tail = node;
        } else {
            node->intrusive_next = m_.head;
            m_.head = node;
        }
    }

    void append(intrusive_forward_list&& other) {
        if (empty()) {
            m_ = std::exchange(other.m_, m_type{});
        } else {
            m_.tail->intrusive_next = std::exchange(other.m_.head, nullptr);
            m_.tail = std::exchange(other.m_.tail, nullptr);
            m_.size += std::exchange(other.m_.size, 0);
        }
        DEBUG_ASSERT(other.m_ == m_type());
    }

    T* pop_front() {
        if (empty()) {
            return nullptr;
        }
        --m_.size;

        node_type* prev_head = m_.head;
        if (m_.head == m_.tail) {
            m_.head = m_.tail = nullptr;
        } else {
            m_.head = m_.head->intrusive_next;
        }
        prev_head->intrusive_next = nullptr;

        return prev_head->downcast();
    }

    void clear() { m_ = m_type{}; }
    auto size() const { return m_.size; }
    bool empty() const { return m_.head == nullptr; }

    auto begin() { return iterator<false>{ m_.head }; }
    auto begin() const { return iterator<true>{ m_.head }; }

    auto end() { return iterator<false>{ nullptr }; }
    auto end() const { return iterator<true>{ nullptr }; }

private:
    struct m_type {
        node_type* head = nullptr;
        node_type* tail = nullptr;
        std::size_t size = 0;

        bool operator==(const m_type&) const = default;
    };
    m_type m_;
};

template <typename T>
template <bool is_const>
class intrusive_forward_list<T>::iterator {
    using qualified_node_type = add_const_if_t<node_type, is_const>;

    friend class intrusive_forward_list<T>;
    explicit iterator(qualified_node_type* node) : node_{ node } {}

public:
    auto operator++() {
        node_ = node_->intrusive_next;
        return *this;
    }
    auto operator++(int) {
        iterator tmp{ node_ };
        node_ = node_->intrusive_next;
        return tmp;
    }

    auto& operator*() const { return *node_->downcast(); }
    auto* operator->() const { return node_->downcast(); }

    template <bool other_is_const>
    bool operator==(const iterator<other_is_const>& other) const {
        return node_ == other.node_;
    }

private:
    qualified_node_type* node_;
};

} // namespace sl::meta
