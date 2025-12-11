//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/monad/result.hpp"

#include <tuple>

namespace sl::meta {
namespace detail {

template <typename ErrT, typename OkT, typename... OkTs>
constexpr ErrT first_err(result<OkT, ErrT>&& r, result<OkTs, ErrT>&&... rs) {
    if (r.is_err()) {
        return std::move(r).err();
    }
    return first_err<ErrT>(std::move(rs)...);
}

template <typename ErrT>
constexpr ErrT first_err() {
    UNREACHABLE("one of rs has to have err");
}

} // namespace detail

template <typename ErrT, typename... OkTs>
constexpr result<std::tuple<OkTs...>, ErrT> all(result<OkTs, ErrT>&&... rs) {
    if ((rs.is_ok() && ... && true)) {
        return ok(std::tuple<OkTs...>{ std::move(rs).ok()... });
    } else {
        return err(detail::first_err<ErrT>(std::move(rs)...));
    }
}

} // namespace sl::meta
