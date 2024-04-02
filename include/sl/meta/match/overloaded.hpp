//
// Created by usatiynyan.
// from https://en.cppreference.com/w/cpp/utility/variant/visit
//

#pragma once

namespace sl::meta {

template <typename... Fs>
struct overloaded : Fs... {
    using Fs::operator()...;
};

template <typename... Fs>
overloaded(Fs...) -> overloaded<Fs...>;

} // namespace sl::meta
