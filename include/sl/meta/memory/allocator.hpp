//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/func/function.hpp"
#include "sl/meta/traits/unique.hpp"

namespace sl::meta {

template <typename AllocatorT>
concept Allocator = requires(AllocatorT) { typename AllocatorT::value_type; }
                    && requires(AllocatorT allocator, typename AllocatorT::value_type* p, std::size_t n) {
                           { allocator.allocate(n) } -> std::same_as<typename AllocatorT::value_type*>;
                           { allocator.deallocate(p, n) };
                       };

template <typename T>
struct allocator_state : immovable {
    using allocate_type = unique_function<T*(std::size_t)>;
    using deallocate_type = unique_function<void(T*, std::size_t)>;

    allocate_type allocate;
    deallocate_type deallocate;
};

template <typename T>
struct allocator {
    using value_type = T;

public:
    explicit allocator(allocator_state<T>& state) : state_ptr_{ &state } {}

    value_type* allocate(std::size_t n) { return state_ptr_->allocate(n); }
    void deallocate(value_type* p, std::size_t n) { return state_ptr_->deallocate(p, n); }

private:
    allocator_state<T>* state_ptr_;
};

template <
    Allocator AllocatorT,
    std::invocable<std::size_t> ObserveAlloc,
    std::invocable<typename AllocatorT::value_type*, std::size_t> ObserveDealloc>
auto decorate_allocator(const AllocatorT& inner_allocator, ObserveAlloc observe_alloc, ObserveDealloc observe_dealloc) {
    using T = typename AllocatorT::value_type;
    return allocator_state<T>{
        .allocate = [inner_allocator = inner_allocator,
                     observe_alloc = std::move(observe_alloc)](std::size_t n) mutable -> T* {
            observe_alloc(n);
            return inner_allocator.allocate(n);
        },
        .deallocate =
            [inner_allocator = inner_allocator,
             observe_dealloc = std::move(observe_dealloc)](T* p, std::size_t n) mutable {
                observe_dealloc(p, n);
                return inner_allocator.deallocate(p, n);
            },
    };
}

} // namespace sl::meta
