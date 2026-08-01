#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

/*
 * Inspired by https://skypjack.github.io/2019-01-25-delegate-revised/
 */
namespace onyx {
/*! @brief Disambiguation tag for constructors and the like. */
template < auto >
struct connect_arg_t {
    /*! @brief Default constructor. */
    explicit connect_arg_t() = default;
};

/**
 * @brief Transcribes the constness of a type to another type.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template < typename To, typename From >
struct constness_as {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = std::remove_const_t< To >;
};

/*! @copydoc constness_as */
template < typename To, typename From >
struct constness_as< To, const From > {
    /*! @brief The type resulting from the transcription of the constness. */
    using type = const To;
};

/**
 * @brief Alias template to facilitate the transcription of the constness.
 * @tparam To The type to which to transcribe the constness.
 * @tparam From The type from which to transcribe the constness.
 */
template < typename To, typename From >
using constness_as_t = typename constness_as< To, From >::type;

namespace Internal {
template < typename Ret, typename... Args >
constexpr auto function_pointer( Ret ( * )( Args... ) ) -> Ret ( * )( Args... );

template < typename Ret, typename Type, typename... Args, typename Other >
constexpr auto function_pointer( Ret ( * )( Type, Args... ), Other&& ) -> Ret ( * )( Args... );

template < typename Class, typename Ret, typename... Args, typename... Other >
constexpr auto function_pointer( Ret ( Class::* )( Args... ), Other&&... ) -> Ret ( * )( Args... );

template < typename Class, typename Ret, typename... Args, typename... Other >
constexpr auto function_pointer( Ret ( Class::* )( Args... ) const, Other&&... ) -> Ret ( * )( Args... );

template < typename Class, typename Type, typename... Other >
constexpr auto function_pointer( Type Class::*, Other&&... ) -> Type ( * )();

template < typename... Type >
using function_pointer_t = decltype( function_pointer( std::declval< Type >()... ) );

template < typename... Class, typename Ret, typename... Args >
[[nodiscard]] constexpr auto index_sequence_for( Ret ( * )( Args... ) ) {
    return std::index_sequence_for< Class..., Args... >{};
}
} // namespace Internal

/**
 * @brief Basic delegate implementation.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 */
template < typename >
class Callback;

/**
 * @brief Utility class to use to send around functions and members.
 *
 * Unmanaged delegate for function pointers and members. Users of this class are
 * in charge of disconnecting instances before deleting them.
 *
 * A delegate can be used as a general purpose invoker without memory overhead
 * for free functions possibly with payloads and bound or unbound members.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template < typename Ret, typename... Args >
class Callback< Ret( Args... ) > {
    template < auto Candidate, std::size_t... Index >
    [[nodiscard]] auto wrap( std::index_sequence< Index... > ) noexcept {
        return []( const void*, Args... args ) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple( std::forward< Args >( args )... );

            if constexpr( std::is_invocable_r_v< Ret,
                                                 decltype( Candidate ),
                                                 typelist_element_t< Index, TypeList< Args... > >... > ) {
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 std::forward< typelist_element_t< Index, TypeList< Args... > > >(
                                     std::get< Index >( arguments ) )... ) );
            } else {
                constexpr auto Offset = sizeof...( Args ) - sizeof...( Index );
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 std::forward< typelist_element_t< Index + Offset, TypeList< Args... > > >(
                                     std::get< Index + Offset >( arguments ) )... ) );
            }
        };
    }

    template < auto Candidate, typename Type, std::size_t... Index >
    [[nodiscard]] auto wrap( Type&, std::index_sequence< Index... > ) noexcept {
        return []( const void* payload, Args... args ) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple( std::forward< Args >( args )... );
            Type* curr = static_cast< Type* >( const_cast< constness_as_t< void, Type >* >( payload ) );

            if constexpr( std::is_invocable_r_v< Ret,
                                                 decltype( Candidate ),
                                                 Type&,
                                                 typelist_element_t< Index, TypeList< Args... > >... > ) {
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 *curr,
                                 std::forward< typelist_element_t< Index, TypeList< Args... > > >(
                                     std::get< Index >( arguments ) )... ) );
            } else {
                constexpr auto Offset = sizeof...( Args ) - sizeof...( Index );
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 *curr,
                                 std::forward< typelist_element_t< Index + Offset, TypeList< Args... > > >(
                                     std::get< Index + Offset >( arguments ) )... ) );
            }
        };
    }

    template < auto Candidate, typename Type, std::size_t... Index >
    [[nodiscard]] auto wrap( Type*, std::index_sequence< Index... > ) noexcept {
        return []( const void* payload, Args... args ) -> Ret {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple( std::forward< Args >( args )... );
            Type* curr = static_cast< Type* >( const_cast< constness_as_t< void, Type >* >( payload ) );

            if constexpr( std::is_invocable_r_v< Ret,
                                                 decltype( Candidate ),
                                                 Type*,
                                                 typelist_element_t< Index, TypeList< Args... > >... > ) {
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 curr,
                                 std::forward< typelist_element_t< Index, TypeList< Args... > > >(
                                     std::get< Index >( arguments ) )... ) );
            } else {
                constexpr auto Offset = sizeof...( Args ) - sizeof...( Index );
                return static_cast< Ret >(
                    std::invoke( Candidate,
                                 curr,
                                 std::forward< typelist_element_t< Index + Offset, TypeList< Args... > > >(
                                     std::get< Index + Offset >( arguments ) )... ) );
            }
        };
    }

  public:
    /*! @brief Function type of the contained target. */
    using FunctionType = Ret( const void*, Args... );
    /*! @brief Function type of the delegate. */
    using Type = Ret( Args... );
    /*! @brief Return type of the delegate. */
    using ResultType = Ret;

    /*! @brief Default constructor. */
    Callback() noexcept
        : m_instance{ nullptr }
        , m_function{ nullptr } {}

    /**
     * @brief Constructs a delegate with a given object or payload, if any.
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload, if any.
     * @param value_or_instance Optional valid object that fits the purpose.
     */
    template < auto Candidate, typename... Type >
    Callback( connect_arg_t< Candidate >, Type&&... valueOrInstance ) noexcept {
        connect< Candidate >( std::forward< Type >( valueOrInstance )... );
    }

    /**
     * @brief Constructs a delegate and connects an user defined function with
     * optional payload.
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    explicit Callback( FunctionType* function, const void* payload = nullptr ) noexcept {
        connect( function, payload );
    }

    /**
     * @brief Connects a free function or an unbound member to a delegate.
     * @tparam Candidate Function or member to connect to the delegate.
     */
    template < auto Candidate >
    void connect() noexcept {
        m_instance = nullptr;

        if constexpr( std::is_invocable_r_v< Ret, decltype( Candidate ), Args... > ) {
            m_function = []( const void*, Args... args ) -> Ret {
                return Ret( std::invoke( Candidate, std::forward< Args >( args )... ) );
            };
        } else if constexpr( std::is_member_pointer_v< decltype( Candidate ) > ) {
            m_function = wrap< Candidate >(
                Internal::index_sequence_for< typelist_element_t< 0, TypeList< Args... > > >(
                    Internal::function_pointer_t< decltype( Candidate ) >{} ) );
        } else {
            m_function = wrap< Candidate >(
                Internal::index_sequence_for( Internal::function_pointer_t< decltype( Candidate ) >{} ) );
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of the instance overcomes
     * the one of the delegate.<br/>
     * When used to connect a free function with payload, its signature must be
     * such that the instance is the first argument before the ones used to
     * define the delegate itself.
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid reference that fits the purpose.
     */
    template < auto Candidate, typename Type >
    void connect( Type& valueOrInstance ) noexcept {
        m_instance = &valueOrInstance;

        if constexpr( std::is_invocable_r_v< Ret, decltype( Candidate ), Type&, Args... > ) {
            m_function = []( const void* payload, Args... args ) -> Ret {
                Type* curr = static_cast< Type* >( const_cast< constness_as_t< void, Type >* >( payload ) );
                return Ret( std::invoke( Candidate, *curr, std::forward< Args >( args )... ) );
            };
        } else {
            m_function = wrap< Candidate >(
                valueOrInstance,
                Internal::index_sequence_for( Internal::function_pointer_t< decltype( Candidate ), Type >{} ) );
        }
    }

    /**
     * @brief Connects a free function with payload or a bound member to a
     * delegate.
     *
     * @sa connect(Type &)
     *
     * @tparam Candidate Function or member to connect to the delegate.
     * @tparam Type Type of class or type of payload.
     * @param value_or_instance A valid pointer that fits the purpose.
     */
    template < auto Candidate, typename Type >
    void connect( Type* valueOrInstance ) noexcept {
        m_instance = valueOrInstance;

        if constexpr( std::is_invocable_r_v< Ret, decltype( Candidate ), Type*, Args... > ) {
            m_function = []( const void* payload, Args... args ) -> Ret {
                Type* curr = static_cast< Type* >( const_cast< constness_as_t< void, Type >* >( payload ) );
                return Ret( std::invoke( Candidate, curr, std::forward< Args >( args )... ) );
            };
        } else {
            m_function = wrap< Candidate >(
                valueOrInstance,
                Internal::index_sequence_for( Internal::function_pointer_t< decltype( Candidate ), Type >{} ) );
        }
    }

    /**
     * @brief Connects an user defined function with optional payload to a
     * delegate.
     *
     * The delegate isn't responsible for the connected object or the payload.
     * Users must always guarantee that the lifetime of an instance overcomes
     * the one of the delegate.<br/>
     * The payload is returned as the first argument to the target function in
     * all cases.
     *
     * @param function Function to connect to the delegate.
     * @param payload User defined arbitrary data.
     */
    void connect( FunctionType* function, const void* payload = nullptr ) noexcept {
        ONYX_ASSERT( function != nullptr, "Uninitialized function pointer" );
        m_instance = payload;
        m_function = function;
    }

    /**
     * @brief Resets a delegate.
     *
     * After a reset, a delegate cannot be invoked anymore.
     */
    void reset() noexcept {
        m_instance = nullptr;
        m_function = nullptr;
    }

    /**
     * @brief Returns a pointer to the stored callable function target, if any.
     * @return An opaque pointer to the stored callable function target.
     */
    [[nodiscard]] FunctionType* target() const noexcept { return m_function; }

    /**
     * @brief Returns the instance or the payload linked to a delegate, if any.
     * @return An opaque pointer to the underlying data.
     */
    [[nodiscard]] const void* data() const noexcept { return m_instance; }

    /**
     * @brief Triggers a delegate.
     *
     * The delegate invokes the underlying function and returns the result.
     *
     * @warning
     * Attempting to trigger an invalid delegate results in undefined
     * behavior.
     *
     * @param args Arguments to use to invoke the underlying function.
     * @return The value returned by the underlying function.
     */
    Ret operator()( Args... args ) const {
        ONYX_ASSERT( static_cast< bool >( *this ), "Uninitialized delegate" );
        return m_function( m_instance, std::forward< Args >( args )... );
    }

    /**
     * @brief Checks whether a delegate actually stores a listener.
     * @return False if the delegate is empty, true otherwise.
     */
    [[nodiscard]] explicit operator bool() const noexcept {
        // no need to also test instance
        return !( m_function == nullptr );
    }

    /**
     * @brief Compares the contents of two delegates.
     * @param other Delegate with which to compare.
     * @return False if the two contents differ, true otherwise.
     */
    [[nodiscard]] bool operator==( const Callback< Ret( Args... ) >& other ) const noexcept {
        return m_function == other.m_function && m_instance == other.m_instance;
    }

  private:
    const void* m_instance = nullptr;
    FunctionType* m_function = nullptr;
};

/**
 * @brief Compares the contents of two delegates.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @param lhs A valid delegate object.
 * @param rhs A valid delegate object.
 * @return True if the two contents differ, false otherwise.
 */
template < typename Ret, typename... Args >
[[nodiscard]] bool operator!=( const Callback< Ret( Args... ) >& lhs, const Callback< Ret( Args... ) >& rhs ) noexcept {
    return !( lhs == rhs );
}

/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 */
template < auto Candidate >
Callback( connect_arg_t< Candidate > )
    -> Callback< std::remove_pointer_t< Internal::function_pointer_t< decltype( Candidate ) > > >;

/**
 * @brief Deduction guide.
 * @tparam Candidate Function or member to connect to the delegate.
 * @tparam Type Type of class or type of payload.
 */
template < auto Candidate, typename Type >
Callback( connect_arg_t< Candidate >, Type&& )
    -> Callback< std::remove_pointer_t< Internal::function_pointer_t< decltype( Candidate ), Type > > >;

/**
 * @brief Deduction guide.
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 */
template < typename Ret, typename... Args >
Callback( Ret ( * )( const void*, Args... ), const void* = nullptr ) -> Callback< Ret( Args... ) >;
} // namespace onyx
