#pragma once

namespace detail
{
    template<typename T, typename Tuple, std::size_t... I>
    constexpr std::unique_ptr<T> make_new_from_tuple_impl(Tuple&& t, std::index_sequence<I...>)
    {
        static_assert(std::is_constructible_v<T, decltype(std::get<I>(std::declval<Tuple>()))...>, "Constructor does not match tuple.");
        return std::make_unique<T>(std::get<I>(std::forward<Tuple>(t))...);
    }

    template<class F, class Tuple, std::size_t... I>
    constexpr auto forward_tuple_unpacked_impl(F&& functor, Tuple&& t, std::index_sequence<I...>)
    {
        static_assert(std::is_invocable_v<F, decltype(std::get<I>(std::declval<Tuple>()))...>, "Functor does not match tuple.");
        return std::apply(functor, std::get<I>(std::forward<Tuple>(t))...);
    }
} // namespace detail

namespace Onyx
{
    using onyxChar8 = char;
    using onyxChar16 = char16_t;
    using onyxChar32 = char32_t;

    using onyxS8 = std::int8_t;
    using onyxS16 = std::int16_t;
    using onyxS32 = std::int32_t;
    using onyxS64 = std::int64_t;
    
    using onyxU8 = std::uint8_t;
    using onyxU16 = std::uint16_t;
    using onyxU32 = std::uint32_t;
    using onyxU64 = std::uint64_t;
    using onyxU128 = std::pair<onyxU64, onyxU64>;

    constexpr onyxS32 ONYX_INVALID_INDEX = -1;

    using onyxPtrDiff = std::ptrdiff_t;

    using onyxF32 = float;
    using onyxF64 = double;

    static constexpr onyxS8 INVALID_INDEX_8 = -1;
    static constexpr onyxS16 INVALID_INDEX_16 = -1;
    static constexpr onyxS32 INVALID_INDEX_32 = -1;
    static constexpr onyxS64 INVALID_INDEX_64 = -1;

    static constexpr onyxU8 onyxMax_U8 = std::numeric_limits<onyxU8>::max();
    static constexpr onyxU16 onyxMax_U16 = std::numeric_limits<onyxU16>::max();
    static constexpr onyxU32 onyxMax_U32 = std::numeric_limits<onyxU32>::max();
    static constexpr onyxU64 onyxMax_U64 = std::numeric_limits<onyxU64>::max();

    static constexpr onyxS8 onyxMax_S8 =  std::numeric_limits<onyxS8>::max();
    static constexpr onyxS16 onyxMax_S16 = std::numeric_limits<onyxS16>::max();
    static constexpr onyxS32 onyxMax_S32 = std::numeric_limits<onyxS32>::max();
    static constexpr onyxS64 onyxMax_S64 = std::numeric_limits<onyxS64>::max();

    using String = std::string;
    using StringView = std::string_view;

    using String_U16 = std::u16string;
    using StringView_U16 = std::u16string_view;

    using String_U32 = std::u32string;
    using StringView_U32 = std::u32string_view;

    using StringId = onyxU32;

    template <typename T>
    using Atomic = std::atomic<T>;

    template <typename T>
    using UniquePtr = std::unique_ptr<T>;

    template <typename T>
    using SharedPtr = std::shared_ptr<T>;

    template <class _Ty, class... _Types>
    constexpr UniquePtr<_Ty> MakeUnique(_Types&&... args) { return std::make_unique<_Ty>(std::forward<_Types>(args)...); }

    template <typename... _Types>
    using Variant = std::variant<_Types...>;

    template <typename T>
    using Optional = std::optional<T>;

    template <typename... _Types>
    using Tuple = std::tuple<_Types...>;

    template<typename T, typename Tuple>
    constexpr UniquePtr<T> MakeUniqueFromTuple(Tuple&& t)
    {
        return detail::make_new_from_tuple_impl<T>(std::forward<Tuple>(t),
            std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }

    template<typename F, typename Tuple>
    constexpr auto ForwardTupleUnpacked(F&& functor, Tuple&& t)
    {
        return detail::forward_tuple_unpacked_impl(std::forward<F>(functor), std::forward<Tuple>(t),
            std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }
    //template <typename T>
    //using Handle = std::weak_ptr<T>;

    //template <typename T>
    //using Reference = std::shared_ptr<T>;

    template <typename T, size_t N>
    using Array = std::array<T, N>;

    template <typename T>
    using DynamicArray = std::vector<T>;

    template <typename KeyT, typename ValueT, class _Hasher = std::hash<KeyT>>
    using HashMap = std::unordered_map<KeyT, ValueT, _Hasher>;

    template <typename T>
    using Set = std::set<T>;

    template <typename T>
    using HashSet = std::unordered_set<T>;

    template <typename T>
    using Stack = std::stack<T>;

    template <typename T>
    using Queue = std::queue<T>;

    template<typename Type, typename = std::allocator<void>>
    class Signal;

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr onyxF64 ToRadians(T deg)
    {
       return static_cast<onyxF64>(deg * (std::numbers::pi_v<long double> / 180));
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr onyxF64 ToDegrees(T radians)
    {
        return static_cast<onyxF64>(radians * (180 / std::numbers::pi_v<long double>));
    }

    // Converts degrees to radians
    constexpr onyxF64 operator""_deg(long double deg)
    {
        return ToRadians(deg);
    }

    constexpr onyxF64 operator""_rad(long double radians)
    {
        return static_cast<onyxF64>(radians);
    }

    template<template <typename...> class Primary, typename T>
    struct is_specialization_of : std::false_type {};

    template<template <typename...> class Primary, typename... Args>
    struct is_specialization_of<Primary, Primary<Args...>> : std::true_type {};

    template <template<typename...> class Primary, typename T>
    constexpr bool is_specialization_of_v = is_specialization_of<Primary, T>::value;
}