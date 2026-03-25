#pragma once

namespace detail {
template < typename T, typename Tuple, std::size_t... I >
constexpr std::unique_ptr< T > make_new_from_tuple_impl( Tuple&& t, std::index_sequence< I... > ) { // NOLINT
    static_assert( std::is_constructible_v< T, decltype( std::get< I >( std::declval< Tuple >() ) )... >,
                   "Constructor does not match tuple." );
    return std::make_unique< T >( std::get< I >( std::forward< Tuple >( t ) )... );
}

template < class F, class Tuple, std::size_t... I >
constexpr auto forward_tuple_unpacked_impl( F&& functor, Tuple&& t, std::index_sequence< I... > ) { // NOLINT
    static_assert( std::is_invocable_v< F, decltype( std::get< I >( std::declval< Tuple >() ) )... >,
                   "Functor does not match tuple." );
    return std::apply( functor, std::get< I >( std::forward< Tuple >( t ) )... );
}
} // namespace detail

namespace onyx {
using float32 = float;  // NOLINT
using float64 = double; // NOLINT

static constexpr int8_t InvalidIndex8 = -1;
static constexpr int16_t InvalidIndex16 = -1;
static constexpr int32_t InvalidIndex32 = -1;
static constexpr int64_t InvalidIndex64 = -1;

using String = std::string;
using StringView = std::string_view;

using StringU16 = std::u16string;
using StringViewU16 = std::u16string_view;

using StringU32 = std::u32string;
using StringViewU32 = std::u32string_view;

using FilePath = std::filesystem::path;

template < typename T >
using Atomic = std::atomic< T >;

template < typename T >
using UniquePtr = std::unique_ptr< T >;

template < typename T >
using SharedPtr = std::shared_ptr< T >;

template < class Ty, class... Types >
constexpr UniquePtr< Ty > makeUnique( Types&&... args ) {
    return std::make_unique< Ty >( std::forward< Types >( args )... );
}

template < typename... Types >
using Variant = std::variant< Types... >;

template < typename T >
using Optional = std::optional< T >;

template < typename... Types >
using Tuple = std::tuple< Types... >;

template < typename T >
using Span = std::span< T >;

template < typename T, typename Tuple >
constexpr UniquePtr< T > makeUniqueFromTuple( Tuple&& t ) {
    return ::detail::make_new_from_tuple_impl< T >(
        std::forward< Tuple >( t ),
        std::make_index_sequence< std::tuple_size_v< std::remove_reference_t< Tuple > > >{} );
}

template < typename F, typename Tuple >
constexpr auto forwardTupleUnpacked( F&& functor, Tuple&& t ) {
    return ::detail::forward_tuple_unpacked_impl(
        std::forward< F >( functor ),
        std::forward< Tuple >( t ),
        std::make_index_sequence< std::tuple_size_v< std::remove_reference_t< Tuple > > >{} );
}
// template <typename T>
// using Handle = std::weak_ptr<T>;

// template <typename T>
// using Reference = std::shared_ptr<T>;

template < typename T, size_t N >
using Array = std::array< T, N >;

template < typename T >
using DynamicArray = std::vector< T >;

template < typename KeyT, typename ValueT, class Hasher = std::hash< KeyT > >
using HashMap = std::unordered_map< KeyT, ValueT, Hasher >;

template < typename T >
using Set = std::set< T >;

template < typename T, typename Hasher = std::hash< T >, typename Keyeq = std::equal_to< T > >
using HashSet = std::unordered_set< T, Hasher, Keyeq >;

template < typename T >
using Stack = std::stack< T >;

template < typename T >
using Queue = std::queue< T >;

template < typename T >
using Function = std::function< T >;

template < typename Type, typename = std::allocator< void > >
class Signal;

template < template < typename... > class Primary, typename T >
struct is_specialization_of : std::false_type {}; // NOLINT

template < template < typename... > class Primary, typename... Args >
struct is_specialization_of< Primary, Primary< Args... > > : std::true_type {};

template < template < typename... > class Primary, typename T >
constexpr bool is_specialization_of_v = is_specialization_of< Primary, T >::value; // NOLINT

template < typename T >
concept Numeric = std::is_arithmetic_v< T >;

template < typename Fn, typename... Args >
concept Invokable = std::is_invocable_v< Fn, Args... >;

template < typename T >
concept ScopedEnum = std::is_scoped_enum_v< T >;
} // namespace onyx