#pragma once

// Reference from https://gist.github.com/daniel-j-h/1ac3726279c3e621f64b
namespace Onyx
{
    template <typename... Args> struct TypeList { };

    template <typename T>
    struct is_type_list : std::false_type {};

    template <typename... Args>
    struct is_type_list<TypeList<Args...>> : std::true_type {};

    template <typename T>
    inline constexpr bool is_type_list_v = is_type_list<T>::value;

    namespace Detail {

        // Forward declare, in order to pattern match via specialization
        template <typename ...> struct TypeVisitor;

        // Pattern match to extract the TypeLists types into Args
        template <template <typename ...> class Sequence, typename... Args>
        struct TypeVisitor<Sequence<Args...>> {

            template <typename F>
            static constexpr void Visit(F&& f) {
                // Start recursive visitation for each type
                DoVisit<F, Args...>(std::forward<F>(f));
            }

            template <typename F>
            static constexpr auto Collect(F&& f)
            {
                return std::tuple<Args...>(f.template operator()<Args>() ...);
            }

            // Allow empty sequence
            template <typename F>
            static constexpr void DoVisit(F&&) { }

            // Base case: one type, invoke functor
            template <typename F, typename Head>
            static constexpr void DoVisit(F&& f) {
                f.template operator() < Head > ();
            }

            // At least [Head, Next], Tail... can be empty
            template <typename F, typename Head, typename Next, typename... Tail>
            static constexpr void DoVisit(F&& f) {
                // Visit head and recurse visitation on rest
                DoVisit<F, Head>(std::forward<F>(f));
                DoVisit<F, Next, Tail...>(std::forward<F>(f));
            }
        };

        template <typename... Args>
        std::tuple<Args...> to_tuple(TypeList<Args...>);

        template <typename... Args>
        std::tuple<Args&...> to_tuple_ref(TypeList<Args...>);

        template <typename TypleList>
        struct typelist_to_tuple
        {
            using type = decltype(to_tuple(std::declval<TypleList>()));
        };

        template <typename TypleList>
        struct typelist_to_tuple_ref
        {
            using type = decltype(to_tuple_ref(std::declval<TypleList>()));
        };
    } // End Detail

    // Invokes the functor with every type, this code generation is done at compile time
    template <typename Sequence, typename F>
    constexpr void ForEach(F&& f) { Detail::TypeVisitor<Sequence>::template Visit(std::forward<F>(f)); }

    template <typename Sequence, typename F>
    constexpr auto ForEachAndCollect(F&& f) { return Detail::TypeVisitor<Sequence>::template Collect(std::forward<F>(f)); }

    template <typename TypeList>
    using typelist_to_tuple_t = typename Detail::typelist_to_tuple<TypeList>::type;

    template <typename TypeList>
    using typelist_to_tuple_ref_t = typename Detail::typelist_to_tuple_ref<TypeList>::type;

    /*! @brief Primary template isn't defined on purpose. */
    template<std::size_t, typename>
    struct typelist_element;

    /**
     * @brief Provides compile-time indexed access to the types of a type list.
     * @tparam Index Index of the type to return.
     * @tparam First First type provided by the type list.
     * @tparam Other Other types provided by the type list.
     */
    template<std::size_t Index, typename First, typename... Other>
    struct typelist_element<Index, TypeList<First, Other...>>
        : typelist_element<Index - 1u, TypeList<Other...>> {};

    /**
     * @brief Provides compile-time indexed access to the types of a type list.
     * @tparam First First type provided by the type list.
     * @tparam Other Other types provided by the type list.  
     */
    template<typename First, typename... Other>
    struct typelist_element<0u, TypeList<First, Other...>> {
        /*! @brief Searched type. */
        using type = First;
    };

    /**
     * @brief Helper type.
     * @tparam Index Index of the type to return.
     * @tparam List Type list to search into.
     */
    template<std::size_t Index, typename List>
    using typelist_element_t = typename typelist_element<Index, List>::type;
}