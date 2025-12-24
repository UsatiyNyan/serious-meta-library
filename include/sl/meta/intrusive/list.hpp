//
// Created by usatiynyan.
// Courtesy of millions of examples from the internet and then some.
// non-owning intrusive list
//

#pragma once

#include "sl/meta/type/const.hpp"

#include <libassert/assert.hpp>

#include <utility>

namespace sl::meta {

template <typename T>
struct intrusive_list_node {
    T* downcast() { return static_cast<T*>(this); }
    const T* downcast() const { return static_cast<T*>(this); }

    intrusive_list_node* intrusive_prev = nullptr;
    intrusive_list_node* intrusive_next = nullptr;

    static void connect(intrusive_list_node* prev, intrusive_list_node* next) {
        if (DEBUG_ASSERT_VAL(prev)) {
            prev->intrusive_next = next;
        }
        if (DEBUG_ASSERT_VAL(next)) {
            next->intrusive_prev = prev;
        }
    }

    static void disconnect(intrusive_list_node* prev, intrusive_list_node* next) {
        DEBUG_ASSERT(prev && next);
        DEBUG_ASSERT(prev->intrusive_next == next);
        DEBUG_ASSERT(next->intrusive_prev == prev);
        prev->intrusive_next = nullptr;
        next->intrusive_prev = nullptr;
    }
};

template <typename T>
struct intrusive_list {
    using node_type = intrusive_list_node<T>;
    template <bool is_const>
    struct iterator;

public:
    intrusive_list() = default;

    intrusive_list(const intrusive_list&) = delete;
    intrusive_list& operator=(const intrusive_list&) = delete;

    intrusive_list(intrusive_list&& other) noexcept : m_{ std::exchange(other.m_, m_type{}) } {}
    intrusive_list& operator=(intrusive_list&& other) {
        m_ = std::exchange(other.m_, m_type{});
        return *this;
    }

    void push_back(node_type* node) {
        DEBUG_ASSERT(node != nullptr);
        ++m_.size;

        node->intrusive_next = nullptr;

        if (empty()) {
            m_.head = m_.tail = node;
        } else {
            node_type::connect(m_.tail, node);
            m_.tail = node;
        }
    }

    void push_front(node_type* node) {
        DEBUG_ASSERT(node != nullptr);
        ++m_.size;

        node->intrusive_prev = nullptr;

        if (empty()) {
            m_.head = m_.tail = node;
        } else {
            node_type::connect(node, m_.head);
            m_.head = node;
        }
    }

    T* pop_back() {
        if (empty()) {
            return nullptr;
        }
        --m_.size;

        node_type* prev_tail = m_.tail;
        if (m_.head == m_.tail) {
            m_.head = m_.tail = nullptr;
        } else {
            m_.tail = m_.tail->intrusive_prev;
            node_type::disconnect(m_.tail, prev_tail);
        }

        return prev_tail->downcast();
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
            node_type::disconnect(prev_head, m_.head);
        }

        return prev_head->downcast();
    }

    T* back() const { return m_.tail == nullptr ? nullptr : m_.tail->downcast(); }
    T* front() const { return m_.head == nullptr ? nullptr : m_.head->downcast(); }

    struct [[nodiscard]] erase_type {
        node_type* prev;
        node_type* next;
    };
    erase_type erase(node_type* node) {
        DEBUG_ASSERT(node != nullptr && !empty());

        erase_type result{
            .prev = node->intrusive_prev,
            .next = node->intrusive_next,
        };
        --m_.size;

        if (node->intrusive_prev && node->intrusive_next) {
            node_type::connect(node->intrusive_prev, node->intrusive_next);
            node->intrusive_prev = nullptr;
            node->intrusive_next = nullptr;
        } else if (node->intrusive_next) {
            DEBUG_ASSERT(!node->intrusive_prev);
            DEBUG_ASSERT(node == m_.head);
            m_.head = m_.head->intrusive_next;
            node_type::disconnect(node, m_.head);
        } else if (node->intrusive_prev) {
            DEBUG_ASSERT(!node->intrusive_next);
            DEBUG_ASSERT(node == m_.tail);
            m_.tail = m_.tail->intrusive_prev;
            node_type::disconnect(m_.tail, node);
        } else {
            DEBUG_ASSERT(!node->intrusive_prev && !node->intrusive_next);
            DEBUG_ASSERT(m_.head == m_.tail && m_.head == node);
            m_.head = m_.tail = nullptr;
        }

        return result;
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
    m_type m_{};
};

template <typename T>
template <bool is_const>
struct intrusive_list<T>::iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    using qualified_node_type = type::add_const_if_t<node_type, is_const>;

public:
    explicit iterator(qualified_node_type* node = nullptr)
        : node_{ node }, //
          next_{ node ? node->intrusive_next : nullptr }, //
          prev_{ node ? node->intrusive_prev : nullptr } {}

    auto operator++() {
        prev_ = node_;
        node_ = next_;
        next_ = next_ ? next_->intrusive_next : nullptr;
        return *this;
    }
    auto operator++(int) {
        iterator tmp = *this;
        operator++();
        return tmp;
    }

    auto operator--() {
        next_ = node_;
        node_ = prev_;
        prev_ = prev_ ? prev_->intrusive_prev : nullptr;
        return *this;
    }
    auto operator--(int) {
        iterator tmp = *this;
        operator--();
        return tmp;
    }

    T& operator*() const { return *node_->downcast(); }
    T* operator->() const { return node_->downcast(); }

    template <bool other_is_const>
    bool operator==(const iterator<other_is_const>& other) const {
        return node_ == other.node_;
    }

private:
    qualified_node_type* node_;
    qualified_node_type* next_;
    qualified_node_type* prev_;
};

} // namespace sl::meta
