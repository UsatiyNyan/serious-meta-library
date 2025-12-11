//
// Created by usatiynyan.
//

#pragma once

#include "sl/meta/traits/invocable.hpp"
#include "sl/meta/traits/is_specialization.hpp"

#include <libassert/assert.hpp>

#include <variant>

namespace sl::meta {

template <typename OkT>
struct ok_t {
    constexpr auto operator<=>(const ok_t&) const = default;

    OkT o;
};

template <typename ErrT>
struct err_t {
    constexpr auto operator<=>(const err_t&) const = default;

    ErrT e;
};

template <typename OkTV>
constexpr auto ok(OkTV&& ov) {
    using OkT = std::decay_t<OkTV>;
    return ok_t<OkT>{ std::forward<OkTV>(ov) };
}

template <typename ErrTV>
constexpr auto err(ErrTV&& ev) {
    using ErrT = std::decay_t<ErrTV>;
    return err_t<ErrT>{ std::forward<ErrTV>(ev) };
}

struct ok_tag_t {};
constexpr ok_tag_t ok_tag;

struct err_tag_t {};
constexpr err_tag_t err_tag;

template <typename OkT, typename ErrT>
struct result {
    using ok_type = OkT;
    using err_type = ErrT;

public: // creation
    constexpr result(ok_t<OkT> v) : oe_{ std::move(v) } {}
    constexpr result(err_t<ErrT> e) : oe_{ std::move(e) } {}

    template <typename... Args>
    constexpr explicit result(ok_tag_t, Args&&... args)
        : oe_{
              ok_t<OkT>{
                  .o{ std::forward<Args>(args)... },
              },
          } {}

    template <typename... Args>
    constexpr explicit result(err_tag_t, Args&&... args)
        : oe_{
              err_t<ErrT>{
                  .e{ std::forward<Args>(args)... },
              },
          } {}

    template <std::convertible_to<OkT> OkU>
    constexpr result(ok_t<OkU> x) : result{ ok_tag, std::move(x.o) } {}
    template <std::convertible_to<ErrT> ErrU>
    constexpr result(err_t<ErrU> x) : result{ err_tag, std::move(x.e) } {}

public: // operators
    constexpr auto operator<=>(const result&) const = default;

public: // getters
    constexpr bool is_ok() const { return std::holds_alternative<ok_t<OkT>>(oe_); }
    constexpr bool is_err() const { return std::holds_alternative<err_t<ErrT>>(oe_); }

    constexpr const OkT& ok() const& {
        DEBUG_ASSERT(is_ok());
        return std::get<ok_t<OkT>>(oe_).o;
    }
    constexpr OkT& ok() & {
        DEBUG_ASSERT(is_ok());
        return std::get<ok_t<OkT>>(oe_).o;
    }
    constexpr const OkT&& ok() const&& {
        DEBUG_ASSERT(is_ok());
        return std::move(std::get<ok_t<OkT>>(oe_).o);
    }
    constexpr OkT&& ok() && {
        DEBUG_ASSERT(is_ok());
        return std::move(std::get<ok_t<OkT>>(oe_).o);
    }

    constexpr const ErrT& err() const& {
        DEBUG_ASSERT(is_err());
        return std::get<err_t<ErrT>>(oe_).e;
    }
    constexpr ErrT& err() & {
        DEBUG_ASSERT(is_err());
        return std::get<err_t<ErrT>>(oe_).e;
    }
    constexpr const ErrT&& err() const&& {
        DEBUG_ASSERT(is_err());
        return std::move(std::get<err_t<ErrT>>(oe_).e);
    }
    constexpr ErrT&& err() && {
        DEBUG_ASSERT(is_err());
        return std::move(std::get<err_t<ErrT>>(oe_).e);
    }

    template <std::same_as<OkT> _ErrT = ErrT>
    constexpr const OkT& both() const& {
        return is_ok() ? ok() : err();
    }
    template <std::same_as<OkT> _ErrT = ErrT>
    constexpr OkT& both() & {
        return is_ok() ? ok() : err();
    }
    template <std::same_as<OkT> _ErrT = ErrT>
    constexpr const OkT&& both() const&& {
        return is_ok() ? std::move(*this).ok() : std::move(*this).err();
    }
    template <std::same_as<OkT> _ErrT = ErrT>
    constexpr OkT&& both() && {
        return is_ok() ? std::move(*this).ok() : std::move(*this).err();
    }

    template <std::convertible_to<OkT> OtherOkTV>
    constexpr OkT ok_or(OtherOkTV&& other) const& {
        return is_ok() ? (*this).ok() : OkT{ std::forward<OtherOkTV>(other) };
    }
    template <std::convertible_to<OkT> OtherOkTV>
    constexpr OkT ok_or(OtherOkTV&& other) && {
        return is_ok() ? std::move(*this).ok() : OkT{ std::forward<OtherOkTV>(other) };
    }
    template <Invocable<OkT> F>
    constexpr OkT ok_or_lazy(F&& f) const& {
        return is_ok() ? (*this).ok() : std::forward<F>(f)();
    }
    template <Invocable<OkT> F>
    constexpr OkT ok_or_lazy(F&& f) && {
        return is_ok() ? std::move(*this).ok() : std::forward<F>(f)();
    }

public: // map, map_err, and_then, or_else, match
    template <typename F>
    constexpr auto map(F&& f) const& {
        return map_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map(F&& f) & {
        return map_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map(F&& f) const&& {
        return map_impl(std::move(*this), std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map(F&& f) && {
        return map_impl(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto map_err(F&& f) const& {
        return map_err_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map_err(F&& f) & {
        return map_err_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map_err(F&& f) const&& {
        return map_err_impl(std::move(*this), std::forward<F>(f));
    }
    template <typename F>
    constexpr auto map_err(F&& f) && {
        return map_err_impl(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto and_then(F&& f) const& {
        return and_then_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto and_then(F&& f) & {
        return and_then_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto and_then(F&& f) const&& {
        return and_then_impl(std::move(*this), std::forward<F>(f));
    }
    template <typename F>
    constexpr auto and_then(F&& f) && {
        return and_then_impl(std::move(*this), std::forward<F>(f));
    }

    template <typename F>
    constexpr auto or_else(F&& f) const& {
        return or_else_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto or_else(F&& f) & {
        return or_else_impl(*this, std::forward<F>(f));
    }
    template <typename F>
    constexpr auto or_else(F&& f) const&& {
        return or_else_impl(std::move(*this), std::forward<F>(f));
    }
    template <typename F>
    constexpr auto or_else(F&& f) && {
        return or_else_impl(std::move(*this), std::forward<F>(f));
    }

    template <typename OkF, typename ErrF>
    constexpr auto match(OkF&& ok_f, ErrF&& err_f) const& {
        return match_impl(*this, std::forward<OkF>(ok_f), std::forward<ErrF>(err_f));
    }
    template <typename OkF, typename ErrF>
    constexpr auto match(OkF&& ok_f, ErrF&& err_f) & {
        return match_impl(*this, std::forward<OkF>(ok_f), std::forward<ErrF>(err_f));
    }
    template <typename OkF, typename ErrF>
    constexpr auto match(OkF&& ok_f, ErrF&& err_f) const&& {
        return match_impl(std::move(*this), std::forward<OkF>(ok_f), std::forward<ErrF>(err_f));
    }
    template <typename OkF, typename ErrF>
    constexpr auto match(OkF&& ok_f, ErrF&& err_f) && {
        return match_impl(std::move(*this), std::forward<OkF>(ok_f), std::forward<ErrF>(err_f));
    }

private:
    template <typename F, typename... Args>
    static constexpr auto invoke_impl(F&& f, Args&&... args) {
        return std::forward<F>(f)(std::forward<Args>(args)...);
    }

    template <
        typename Self,
        typename F,
        typename R = decltype(invoke_impl(std::declval<F>(), std::declval<Self>().ok()))>
    static constexpr result<R, ErrT> map_impl(Self&& self, F&& f) {
        if (self.is_ok()) {
            return result<R, ErrT>{ ok_tag, invoke_impl(std::forward<F>(f), std::forward<Self>(self).ok()) };
        }
        return result<R, ErrT>{ err_tag, std::forward<Self>(self).err() };
    }

    template <
        typename Self,
        typename F,
        typename R = decltype(invoke_impl(std::declval<F>(), std::declval<Self>().err()))>
    static constexpr result<OkT, R> map_err_impl(Self&& self, F&& f) {
        if (self.is_err()) {
            return result<OkT, R>{ err_tag, invoke_impl(std::forward<F>(f), std::forward<Self>(self).err()) };
        }
        return result<OkT, R>{ ok_tag, std::forward<Self>(self).ok() };
    }

    template <
        typename Self,
        typename F,
        IsSpecialization<result> R = decltype(invoke_impl(std::declval<F>(), std::declval<Self>().ok()))>
    static constexpr R and_then_impl(Self&& self, F&& f) {
        if (self.is_ok()) {
            return invoke_impl(std::forward<F>(f), std::forward<Self>(self).ok());
        }
        return R{ err_tag, std::forward<Self>(self).err() };
    }

    template <
        typename Self,
        typename F,
        IsSpecialization<result> R = decltype(invoke_impl(std::declval<F>(), std::declval<Self>().err()))>
    static constexpr R or_else_impl(Self&& self, F&& f) {
        if (self.is_err()) {
            return invoke_impl(std::forward<F>(f), std::forward<Self>(self).err());
        }
        return R{ ok_tag, std::forward<Self>(self).ok() };
    }

    template <typename Self, typename OkF, typename ErrF>
    static constexpr auto match_impl(Self&& self, OkF&& ok_f, ErrF&& err_f) {
        if (self.is_ok()) {
            return invoke_impl(std::forward<OkF>(ok_f), std::forward<Self>(self).ok());
        } else {
            return invoke_impl(std::forward<ErrF>(err_f), std::forward<Self>(self).err());
        }
    }

public: // extras
    template <IsSpecialization<result> _OkT = OkT, std::same_as<typename OkT::err_type> _ErrT = ErrT>
    constexpr result<typename _OkT::ok_type, _ErrT> flatten() && {
        if (is_err()) {
            return result<typename _OkT::ok_type, _ErrT>{ err_tag, std::move(*this).err() };
        }
        return std::move(*this).ok();
    }

private:
    std::variant<ok_t<OkT>, err_t<ErrT>> oe_;
};

} // namespace sl::meta
