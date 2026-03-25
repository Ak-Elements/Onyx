#pragma once

namespace onyx {
namespace enums {
template < typename Enum >
constexpr auto toIntegral( Enum value ) -> std::underlying_type_t< Enum > {
    return static_cast< std::underlying_type_t< Enum > >( value );
}

template < typename Enum >
constexpr auto toEnum( std::underlying_type_t< Enum > value ) -> Enum {
    return static_cast< Enum >( value );
}

template < typename Enum >
constexpr StringView toString( Enum value ) {
    ONYX_ASSERT( enums::toIntegral( value ) >= MAGIC_ENUM_RANGE_MIN );
    ONYX_ASSERT( enums::toIntegral( value ) <= MAGIC_ENUM_RANGE_MAX );
    return magic_enum::enum_name( value );
}

template < typename Enum >
constexpr StringView toString( std::underlying_type_t< Enum > value ) {
    ONYX_ASSERT( value >= MAGIC_ENUM_RANGE_MIN );
    ONYX_ASSERT( value <= MAGIC_ENUM_RANGE_MAX );
    return magic_enum::enum_name( static_cast< Enum >( value ) );
}

template < typename Enum >
constexpr Enum fromString( StringView str ) {
    auto enumVal = magic_enum::enum_cast< Enum >( str, magic_enum::case_insensitive );
    ONYX_ASSERT( enumVal.has_value() );
    return enumVal.value();
}

template < typename Enum >
constexpr Enum fromString( StringView str, Enum defaultValue ) {
    auto enumVal = magic_enum::enum_cast< Enum >( str, magic_enum::case_insensitive );
    return enumVal.value_or( defaultValue );
}

template < typename Enum, typename T >
constexpr bool all( T flags, Enum contains ) {
    const std::underlying_type_t< Enum > containsFlag = toIntegral( contains );
    return ( static_cast< std::underlying_type_t< Enum > >( flags ) & containsFlag ) == containsFlag;
}

template < typename Enum, typename T >
constexpr bool any( T flags, Enum contains ) {
    return ( static_cast< std::underlying_type_t< Enum > >( flags ) & ( toIntegral( contains ) ) ) != 0;
}

template < typename Enum >
constexpr bool all( Enum flags, Enum contains ) {
    const std::underlying_type_t< Enum > containsFlag = toIntegral( contains );
    return ( toIntegral( flags ) & containsFlag ) == containsFlag;
}

template < typename Enum >
constexpr bool none( Enum flags, Enum noneOfFlags ) {
    return ( toIntegral( flags ) & ( toIntegral( noneOfFlags ) ) ) == 0;
}

} // namespace enums

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum& operator--( Enum& value ) // pre decrement
{
    const std::underlying_type_t< Enum > val = enums::toIntegral( value );
    value = static_cast< Enum >( val - 1 );
    return value;
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum& operator|=( Enum& lhs, Enum rhs ) {
    lhs = static_cast< Enum >( enums::toIntegral( lhs ) | enums::toIntegral( rhs ) );
    return lhs;
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum& operator^=( Enum& lhs, Enum rhs ) {
    lhs = static_cast< Enum >( enums::toIntegral( lhs ) ^ enums::toIntegral( rhs ) );
    return lhs;
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum operator|( Enum lhs, Enum rhs ) {
    return static_cast< Enum >( enums::toIntegral( lhs ) | enums::toIntegral( rhs ) );
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum operator&( Enum lhs, Enum rhs ) {
    return static_cast< Enum >( enums::toIntegral( lhs ) & enums::toIntegral( rhs ) );
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum operator^( Enum lhs, Enum rhs ) {
    return static_cast< Enum >( enums::toIntegral( lhs ) ^ enums::toIntegral( rhs ) );
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr bool operator!( Enum value ) {
    return !enums::toIntegral( value );
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr bool operator~( Enum value ) {
    return static_cast< Enum >( ~enums::toIntegral( value ) );
}
} // namespace onyx

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum operator++( Enum& value, int ) // postfix increment
{
    Enum tmp = value;
    const std::underlying_type_t< Enum > val = onyx::enums::toIntegral( value );
    value = static_cast< Enum >( val + 1 );
    return tmp;
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum& operator++( Enum& value ) // pre increment
{
    const std::underlying_type_t< Enum > val = onyx::enums::toIntegral( value );
    value = static_cast< Enum >( val + 1 );
    return value;
}

template < typename Enum > requires std::is_scoped_enum_v< Enum >
constexpr Enum operator--( Enum& value, int ) // postfix decrement
{
    Enum tmp = value;
    const std::underlying_type_t< Enum > val = onyx::enums::toIntegral( value );
    value = static_cast< Enum >( val - 1 );
    return tmp;
}