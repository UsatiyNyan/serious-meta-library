//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/intrusive/forward_list.hpp"

namespace sl::meta {

template <typename T>
intrusive_forward_list_node<T>* intrusive_forward_list_node_reverse(intrusive_forward_list_node<T>* head) {
    intrusive_forward_list_node<T>* prev = nullptr;
    intrusive_forward_list_node<T>* node = head;
    while (node != nullptr) {
        intrusive_forward_list_node<T>* next = node->intrusive_next;
        node->intrusive_next = prev;
        prev = node;
        node = next;
    }
    return prev;
}

template <typename T, typename F>
void intrusive_forward_list_node_for_each(intrusive_forward_list_node<T>* node, F&& f) {
    while (node != nullptr) {
        auto* next = node->intrusive_next;
        std::forward<F>(f)(node->downcast());
        node = next;
    }
}

} // namespace sl::meta
