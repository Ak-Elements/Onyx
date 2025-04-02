#pragma once

// Defines all bitwise operators for enum classes so they can be (mostly) used as a regular flags enum
#define ONYX_ENABLE_BITMASK_OPERATORS(Enum) \
	inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = static_cast<Enum>(Enums::ToIntegral(Lhs) | Enums::ToIntegral(Rhs)); } \
	inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = static_cast<Enum>(Enums::ToIntegral(Lhs) & Enums::ToIntegral(Rhs)); } \
	inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = static_cast<Enum>(Enums::ToIntegral(Lhs) ^ Enums::ToIntegral(Rhs)); } \
	inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return static_cast<Enum>(Enums::ToIntegral(Lhs) | Enums::ToIntegral(Rhs)); } \
	inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return static_cast<Enum>(Enums::ToIntegral(Lhs) & Enums::ToIntegral(Rhs)); } \
	inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return static_cast<Enum>(Enums::ToIntegral(Lhs) ^ Enums::ToIntegral(Rhs)); } \
	inline constexpr bool  operator! (Enum  value) { return !Enums::ToIntegral(value); } \
	inline constexpr Enum  operator~ (Enum  value) { return static_cast<Enum>(~Enums::ToIntegral(value)); }

namespace Onyx::Enums
{
	template <typename Enum>
	constexpr auto ToIntegral(Enum value) -> std::underlying_type_t<Enum>
	{
		return static_cast<std::underlying_type_t<Enum>>(value);
	}

	template <typename Enum>
	constexpr auto ToEnum(std::underlying_type_t<Enum> value) -> Enum
	{
		return static_cast<Enum>(value);
	}

	template <typename Enum>
	constexpr StringView ToString(Enum value)
	{
		ONYX_ASSERT(Enums::ToIntegral(value) >= MAGIC_ENUM_RANGE_MIN);
		ONYX_ASSERT(Enums::ToIntegral(value) <= MAGIC_ENUM_RANGE_MAX);
		return magic_enum::enum_name(value);
	}

	template <typename Enum>
	constexpr StringView ToString(std::underlying_type_t<Enum> value)
	{
		ONYX_ASSERT(value >= MAGIC_ENUM_RANGE_MIN);
		ONYX_ASSERT(value <= MAGIC_ENUM_RANGE_MAX);
		return magic_enum::enum_name(static_cast<Enum>(value));
	}

	template <typename Enum>
	constexpr Enum FromString(const StringView& str)
	{
		auto enumVal = magic_enum::enum_cast<Enum>(str, magic_enum::case_insensitive);
		ONYX_ASSERT(enumVal.has_value());
		return enumVal.value();
	}

	template <typename Enum>
	constexpr Enum FromString(const StringView& str, Enum defaultValue)
	{
		auto enumVal = magic_enum::enum_cast<Enum>(str, magic_enum::case_insensitive);
		return enumVal.value_or(defaultValue);
	}
    
	template<typename Enum, typename T>
	constexpr bool HasAllFlags(T Flags, Enum Contains)
	{
		const std::underlying_type_t<Enum> containsFlag = ToIntegral(Contains);
		return (static_cast<std::underlying_type_t<Enum>>(Flags) & containsFlag) == containsFlag;
	}

	template<typename Enum, typename T>
	constexpr bool HasAnyFlags(T Flags, Enum Contains)
	{
		return (static_cast<std::underlying_type_t<Enum>>(Flags) & (ToIntegral(Contains))) != 0;
	}

	template<typename Enum>
	constexpr bool HasAllFlags(Enum Flags, Enum Contains)
	{
		const std::underlying_type_t<Enum> containsFlag = ToIntegral(Contains);
		return (ToIntegral(Flags) & containsFlag) == containsFlag;
	}

	template<typename Enum>
	constexpr bool HasNoneOf(Enum Flags, Enum noneOfFlags)
	{
		return (ToIntegral(Flags) & (ToIntegral(noneOfFlags))) == 0;
	}

	template<typename Enum>
	constexpr bool HasNotFlags(Enum Flags, Enum Contains)
	{
		return (ToIntegral(Flags) & (ToIntegral(Contains))) != 0;
	}
}

template <typename Enum> requires std::is_enum_v<Enum>
constexpr Enum operator++(Enum& value, int) // postfix increment
{
	Enum tmp = value;
	const std::underlying_type_t<Enum> val = Onyx::Enums::ToIntegral(value);
	value = static_cast<Enum>(val + 1);
	return tmp;
}

template <typename Enum> requires std::is_enum_v<Enum>
constexpr Enum& operator++(Enum& value) // pre increment
{
	const std::underlying_type_t<Enum> val = Onyx::Enums::ToIntegral(value);
	value = static_cast<Enum>(val + 1);
	return value;
}

template <typename Enum> requires std::is_enum_v<Enum>
constexpr Enum operator--(Enum& value, int) // postfix decrement
{
	Enum tmp = value;
	const std::underlying_type_t<Enum> val = Onyx::Enums::ToIntegral(value);
	value = static_cast<Enum>(val - 1);
	return tmp;
}

template <typename Enum> requires std::is_enum_v<Enum>
constexpr Enum& operator--(Enum& value) // pre decrement
{
	const std::underlying_type_t<Enum> val = Onyx::Enums::ToIntegral(value);
	value = static_cast<Enum>(val - 1);
	return value;
}

