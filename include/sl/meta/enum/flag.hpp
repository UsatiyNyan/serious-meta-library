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
    explicit constexpr enum_flag(underlying_type value = underlying_type{}) : value_{ value } {}

    constexpr EnumT get_enum() const { return static_cast<EnumT>(value_); }
    constexpr underlying_type get_underlying() const { return value_; }
    explicit constexpr operator bool() const { return value_ != underlying_type{}; }

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

    // for convenience
    constexpr enum_flag operator|(EnumT other) const { return *this | enum_flag(other); }
    constexpr enum_flag operator&(EnumT other) const { return *this & enum_flag(other); }
    constexpr enum_flag operator^(EnumT other) const { return *this ^ enum_flag(other); }
    constexpr bool operator==(EnumT other) const { return value_ == enum_flag(other); }

private:
    underlying_type value_;
};

} // namespace sl::meta
