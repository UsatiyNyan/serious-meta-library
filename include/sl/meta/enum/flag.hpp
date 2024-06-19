//
// Created by usatiynyan.
//

#pragma once

#include <type_traits>

namespace sl::meta {

// the enum values should look like flags:
// NONE = 0,
// FLAG0 = 1 << 0,
// FLAG1 = 1 << 1,
// FLAG2 = 1 << 2,
// ...
template <typename EnumT>
    requires std::is_enum_v<EnumT>
class enum_flag {
public:
    using underlying_type = std::underlying_type_t<EnumT>;

    explicit constexpr enum_flag(EnumT enum_value) : value_{ static_cast<underlying_type>(enum_value) } {}
    explicit constexpr enum_flag(underlying_type value) : value_{ value } {}

    explicit constexpr operator EnumT() const { return static_cast<EnumT>(value_); }
    explicit constexpr operator underlying_type() const { return value_; }
    constexpr operator bool() const { return value_ != underlying_type{}; }

    constexpr enum_flag operator|(enum_flag other) const {
        return enum_flag{ static_cast<underlying_type>(value_ | other.value_) };
    }
    constexpr enum_flag operator&(enum_flag other) const {
        return enum_flag{ static_cast<underlying_type>(value_ & other.value_) };
    }
    constexpr enum_flag operator^(enum_flag other) const {
        return enum_flag{ static_cast<underlying_type>(value_ ^ other.value_) };
    }
    constexpr enum_flag operator~() const { return enum_flag{ static_cast<underlying_type>(~value_) }; }
    constexpr bool operator==(enum_flag other) const { return value_ == other.value_; }

private:
    underlying_type value_;
};

} // namespace sl::meta
