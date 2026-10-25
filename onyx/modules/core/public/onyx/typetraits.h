#pragma once

namespace onyx {
namespace internal {
template < typename >
struct IsTupleImpl : std::false_type {};

template < typename... Args >
struct IsTupleImpl< std::tuple< Args... > > : std::true_type {};

template < uint64_t... Idxs >
constexpr auto substringAsArray( StringView str, std::index_sequence< Idxs... > ) {
    return Array< char, sizeof...( Idxs ) + 1 >{ str[ Idxs ]..., '\0' };
}

template < typename T >
constexpr auto typeName() {
#if ONYX_IS_CLANG
    constexpr auto Prefix = std::string_view{ "=" };
    constexpr auto Suffix = std::string_view{ "]" };
    constexpr auto Function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_GCC
    constexpr auto prefix = std::string_view{ "=" };
    constexpr auto suffix = std::string_view{ "]" };
    constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_MSVC
    constexpr auto classPrefix = std::string_view{ "class " };
    constexpr auto structPrefix = std::string_view{ "struct " };

    constexpr auto prefix = std::string_view{ "<" };
    constexpr auto suffix = std::string_view{ ">" };
    constexpr auto function = std::string_view{ __FUNCSIG__ };
#else
#error Unsupported compiler
#endif

    constexpr auto First = Function.find_first_not_of( ' ', Function.find_first_of( Prefix ) + 1 );
    constexpr auto Value = Function.substr( First, Function.find_last_of( Suffix ) - First );
    return Value;
}

template < typename T >
struct TypeNameHolder {
    static inline constexpr auto Value = typeName< T >();
};
} // namespace internal

template < typename T >
constexpr StringView typeName() {
    constexpr auto Value = internal::TypeNameHolder< T >::Value;
    return { Value.data(), Value.size() };
}

template < typename T >
constexpr uint32_t typeHash() {
    constexpr auto Value = internal::TypeNameHolder< T >::Value;
    return hash::fnV1aHash< uint32_t >( Value );
}

template < typename T >
constexpr uint64_t typeHash64() {
    constexpr auto Value = internal::TypeNameHolder< T >::Value;
    return hash::fnV1aHash< uint64_t >( Value );
}

template < typename Type >
struct IsTuple : internal::IsTupleImpl< std::remove_cv_t< Type > > {};

template < typename Type >
inline constexpr bool IsTupleV = IsTuple< Type >::value;

template < typename T >
concept HasTypeId = requires( const T obj ) {
    T::TypeId;
    { obj.getTypeId() };
};

template < typename T >
concept HasIsValid = requires( const T obj ) {
    { obj.isValid() } -> std::same_as< bool >;
};
} // namespace onyx
