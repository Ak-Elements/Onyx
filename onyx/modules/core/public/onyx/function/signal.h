#pragma once

#include <onyx/function/callback.h>

/*
 * Inspired by entt::sigh.hpp
 */
namespace onyx {
/**
 * @brief Sink class.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Type A valid signal handler type.
 */
template < typename T >
class Sink;

/**
 * @brief Unmanaged signal handler.
 *
 * Primary template isn't defined on purpose. All the specializations give a
 * compile-time error unless the template parameter is a function type.
 *
 * @tparam Type A valid function type.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template < typename Type, typename Allocator >
class Signal;

/**
 * @brief Unmanaged signal handler.
 *
 * It works directly with references to classes and pointers to member functions
 * as well as pointers to free functions. Users of this class are in charge of
 * disconnecting instances before deleting them.
 *
 * This class serves mainly two purposes:
 *
 * * Creating signals to use later to notify a bunch of listeners.
 * * Collecting results from a set of functions like in a voting system.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template < typename Ret, typename... Args, typename Allocator >
class Signal< Ret( Args... ), Allocator > {
    friend class Sink< Signal< Ret( Args... ), Allocator > >;

    using AllocTraits = std::allocator_traits< Allocator >;
    using DelegateType = Callback< Ret( Args... ) >;
    using ContainerType = std::vector< DelegateType, typename AllocTraits::template rebind_alloc< DelegateType > >;

  public:
    /*! @brief Allocator type. */
    using AllocatorType = Allocator;
    /*! @brief Unsigned integer type. */
    using SizeType = std::size_t;
    /*! @brief Sink type. */
    using SinkType = Sink< Signal< Ret( Args... ), Allocator > >;

    /*! @brief Default constructor. */
    Signal() noexcept( std::is_nothrow_default_constructible_v< AllocatorType > &&
                       std::is_nothrow_constructible_v< ContainerType, const AllocatorType& > )
        : Signal{ AllocatorType{} } {}

    /**
     * @brief Constructs a signal handler with a given allocator.
     * @param allocator The allocator to use.
     */
    explicit Signal( const AllocatorType& allocator ) noexcept(
        std::is_nothrow_constructible_v< ContainerType, const AllocatorType& > )
        : m_callbacks{ allocator } {}

    /**
     * @brief Copy constructor.
     * @param other The instance to copy from.
     */
    Signal( const Signal& other ) noexcept( std::is_nothrow_copy_constructible_v< ContainerType > )
        : m_callbacks{ other.m_callbacks } {}

    /**
     * @brief Allocator-extended copy constructor.
     * @param other The instance to copy from.
     * @param allocator The allocator to use.
     */
    Signal( const Signal& other, const AllocatorType& allocator ) noexcept(
        std::is_nothrow_constructible_v< ContainerType, const ContainerType&, const AllocatorType& > )
        : m_callbacks{ other.m_callbacks, allocator } {}

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    Signal( Signal&& other ) noexcept( std::is_nothrow_move_constructible_v< ContainerType > )
        : m_callbacks{ std::move( other.m_callbacks ) } {}

    /**
     * @brief Allocator-extended move constructor.
     * @param other The instance to move from.
     * @param allocator The allocator to use.
     */
    Signal( Signal&& other, const AllocatorType& allocator ) noexcept(
        std::is_nothrow_constructible_v< ContainerType, ContainerType&&, const AllocatorType& > )
        : m_callbacks{ std::move( other.m_callbacks ), allocator } {}

    /**
     * @brief Copy assignment operator.
     * @param other The instance to copy from.
     * @return This signal handler.
     */
    Signal& operator=( const Signal& other ) noexcept( std::is_nothrow_copy_assignable_v< ContainerType > ) {
        m_callbacks = other.m_callbacks;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This signal handler.
     */
    Signal& operator=( Signal&& other ) noexcept( std::is_nothrow_move_assignable_v< ContainerType > ) {
        m_callbacks = std::move( other.m_callbacks );
        return *this;
    }

    /**
     * @brief Exchanges the contents with those of a given signal handler.
     * @param other Signal handler to exchange the content with.
     */
    void swap( Signal& other ) noexcept( std::is_nothrow_swappable_v< ContainerType > ) {
        using std::swap;
        swap( m_callbacks, other.m_callbacks );
    }

    /**
     * @brief Returns the associated allocator.
     * @return The associated allocator.
     */
    [[nodiscard]] constexpr AllocatorType getAllocator() const noexcept { return m_callbacks.get_allocator(); }

    /**
     * @brief Number of listeners connected to the signal.
     * @return Number of listeners currently connected.
     */
    [[nodiscard]] SizeType size() const noexcept { return m_callbacks.size(); }

    /**
     * @brief Returns false if at least a listener is connected to the signal.
     * @return True if the signal has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const noexcept { return m_callbacks.empty(); }

    /**
     * @brief Triggers a signal.
     *
     * All the listeners are notified. Order isn't guaranteed.
     *
     * @param args Arguments to use to invoke listeners.
     */
    void dispatch( Args... args ) const {
        for( auto pos = m_callbacks.size(); pos; --pos ) {
            m_callbacks[ pos - 1u ]( args... );
        }
    }

    /**
     * @brief Collects return values from the listeners.
     *
     * The collector must expose a call operator with the following properties:
     *
     * * The return type is either `void` or such that it's convertible to
     *   `bool`. In the second case, a true value will stop the iteration.
     * * The list of parameters is empty if `Ret` is `void`, otherwise it
     *   contains a single element such that `Ret` is convertible to it.
     *
     * @tparam Func Type of collector to use, if any.
     * @param func A valid function object.
     * @param args Arguments to use to invoke listeners.
     */
    template < typename Func >
    void collect( Func func, Args... args ) const {
        for( auto pos = m_callbacks.size(); pos; --pos ) {
            if constexpr( std::is_void_v< Ret > || !std::is_invocable_v< Func, Ret > ) {
                m_callbacks[ pos - 1u ]( args... );

                if constexpr( std::is_invocable_r_v< bool, Func > ) {
                    if( func() ) {
                        break;
                    }
                } else {
                    func();
                }
            } else {
                if constexpr( std::is_invocable_r_v< bool, Func, Ret > ) {
                    if( func( m_callbacks[ pos - 1u ]( args... ) ) ) {
                        break;
                    }
                } else {
                    func( m_callbacks[ pos - 1u ]( args... ) );
                }
            }
        }
    }

  private:
    ContainerType m_callbacks;
};

/**
 * @brief Connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.
 */
class Connection {
    template < typename >
    friend class Sink;

    Connection( Callback< void( void* ) > fn, void* ref )
        : m_disconnect{ fn }
        , m_signal{ ref } {}

  public:
    /*! @brief Default constructor. */
    Connection()
        : m_disconnect{}
        , m_signal{} {}

    /**
     * @brief Checks whether a connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const noexcept { return static_cast< bool >( m_disconnect ); }

    /*! @brief Breaks the connection. */
    void release() {
        if( m_disconnect ) {
            m_disconnect( m_signal );
            m_disconnect.reset();
        }
    }

  private:
    Callback< void( void* ) > m_disconnect;
    void* m_signal;
};

/**
 * @brief Scoped connection class.
 *
 * Opaque object the aim of which is to allow users to release an already
 * estabilished connection without having to keep a reference to the signal or
 * the sink that generated it.<br/>
 * A scoped connection automatically breaks the link between the two objects
 * when it goes out of scope.
 */
struct ScopedConnection {
    /*! @brief Default constructor. */
    ScopedConnection() = default;

    /**
     * @brief Constructs a scoped connection from a basic connection.
     * @param other A valid connection object.
     */
    explicit ScopedConnection( const Connection& other )
        : m_connection{ other } {}

    /*! @brief Default copy constructor, deleted on purpose. */
    ScopedConnection( const ScopedConnection& ) = delete;

    /**
     * @brief Move constructor.
     * @param other The scoped connection to move from.
     */
    ScopedConnection( ScopedConnection&& other ) noexcept
        : m_connection{ std::exchange( other.m_connection, {} ) } {}

    /*! @brief Automatically breaks the link on destruction. */
    ~ScopedConnection() { m_connection.release(); }

    /**
     * @brief Default copy assignment operator, deleted on purpose.
     * @return This scoped connection.
     */
    ScopedConnection& operator=( const ScopedConnection& ) = delete;

    /**
     * @brief Move assignment operator.
     * @param other The scoped connection to move from.
     * @return This scoped connection.
     */
    ScopedConnection& operator=( ScopedConnection&& other ) noexcept {
        m_connection = std::exchange( other.m_connection, {} );
        return *this;
    }

    /**
     * @brief Acquires a connection.
     * @param other The connection object to acquire.
     * @return This scoped connection.
     */
    ScopedConnection& operator=( Connection other ) {
        m_connection = other;
        return *this;
    }

    /**
     * @brief Checks whether a scoped connection is properly initialized.
     * @return True if the connection is properly initialized, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const noexcept { return static_cast< bool >( m_connection ); }

    /*! @brief Breaks the connection. */
    void release() { m_connection.release(); }

  private:
    Connection m_connection;
};

/**
 * @brief Sink class.
 *
 * A sink is used to connect listeners to signals and to disconnect them.<br/>
 * The function type for a listener is the one of the signal to which it
 * belongs.
 *
 * The clear separation between a signal and a sink permits to store the former
 * as private data member without exposing the publish functionality to the
 * users of the class.
 *
 * @warning
 * Lifetime of a sink must not overcome that of the signal to which it refers.
 * In any other case, attempting to use a sink results in undefined behavior.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template < typename Ret, typename... Args, typename Allocator >
class Sink< Signal< Ret( Args... ), Allocator > > {
    using SignalType = Signal< Ret( Args... ), Allocator >;
    using DelegateType = typename SignalType::DelegateType;
    using DifferenceType = typename SignalType::ContainerType::difference_type;

    template < auto Candidate, typename Type >
    static void release( Type valueOrInstance, void* signal ) {
        Sink{ *static_cast< SignalType* >( signal ) }.disconnect< Candidate >( valueOrInstance );
    }

    template < auto Candidate >
    static void release( void* signal ) {
        Sink{ *static_cast< SignalType* >( signal ) }.disconnect< Candidate >();
    }

    template < typename Func >
    void disconnectIf( Func callback ) {
        for( auto pos = m_signal->m_callbacks.size(); pos; --pos ) {
            if( auto& elem = m_signal->m_callbacks[ pos - 1u ]; callback( elem ) ) {
                elem = std::move( m_signal->m_callbacks.back() );
                m_signal->m_callbacks.pop_back();
            }
        }
    }

  public:
    /**
     * @brief Constructs a sink that is allowed to modify a given signal.
     * @param ref A valid reference to a signal object.
     */
    explicit Sink( Signal< Ret( Args... ), Allocator >& ref ) noexcept
        : m_signal{ &ref } {
        // signal->m_Callbacks.reserve(5);
    }

    /**
     * @brief Returns false if at least a listener is connected to the sink.
     * @return True if the sink has no listeners connected, false otherwise.
     */
    [[nodiscard]] bool empty() const noexcept { return m_signal->m_callbacks.empty(); }

    [[nodiscard]] auto& signalOrAssert() const noexcept {
        ONYX_ASSERT( m_signal != nullptr, "Invalid pointer to signal" );
        return *m_signal;
    }

    /**
     * @brief Connects a free function (with or without payload), a bound or an
     * unbound member to a signal.
     * @tparam Candidate Function or member to connect to the signal.
     * @tparam Type Type of class or type of payload, if any.
     * @param value_or_instance A valid object that fits the purpose, if any.
     * @return A properly initialized connection object.
     */
    template < auto Candidate >
    Connection connect() {
        disconnect< Candidate >();

        DelegateType call{};
        call.template connect< Candidate >();
        signalOrAssert().m_callbacks.push_back( std::move( call ) );

        Callback< void( void* ) > conn{};
        conn.template connect< &release< Candidate > >();
        return { conn, m_signal };
    }

    template < auto Candidate, typename Type >
    Connection connect( Type& valueOrInstance ) {
        disconnect< Candidate >( valueOrInstance );

        DelegateType call{};
        call.template connect< Candidate >( valueOrInstance );
        signalOrAssert().m_callbacks.push_back( std::move( call ) );

        Callback< void( void* ) > conn{};
        conn.template connect< &release< Candidate, Type& > >( valueOrInstance );
        return { conn, m_signal };
    }

    template < auto Candidate, typename Type >
    Connection connect( Type* valueOrInstance ) {
        disconnect< Candidate >( valueOrInstance );

        DelegateType call{};
        call.template connect< Candidate >( valueOrInstance );
        signalOrAssert().m_callbacks.push_back( std::move( call ) );

        Callback< void( void* ) > conn{};
        conn.template connect< &release< Candidate, Type* > >( valueOrInstance );
        return { conn, m_signal };
    }

    /**
     * @brief Disconnects a free function (with or without payload), a bound or
     * an unbound member from a signal.
     * @tparam Candidate Function or member to disconnect from the signal.
     * @tparam Type Type of class or type of payload, if any.
     * @param value_or_instance A valid object that fits the purpose, if any.
     */
    template < auto Candidate, typename... Type >
    void disconnect( Type&&... valueOrInstance ) {
        DelegateType call{};
        call.template connect< Candidate >( valueOrInstance... );
        disconnectIf( [ &call ]( const auto& elem ) { return elem == call; } );
    }

    /**
     * @brief Disconnects free functions with payload or bound members from a
     * signal.
     * @param value_or_instance A valid object that fits the purpose.
     */
    void disconnect( const void* valueOrInstance ) {
        if( valueOrInstance ) {
            disconnectIf( [ valueOrInstance ]( const auto& elem ) { return elem.data() == valueOrInstance; } );
        }
    }

    /*! @brief Disconnects all the listeners from a signal. */
    void disconnect() { m_signal->m_callbacks.clear(); }

  private:
    SignalType* m_signal;
};

/**
 * @brief Deduction guide.
 *
 * It allows to deduce the signal handler type of a sink directly from the
 * signal it refers to.
 *
 * @tparam Ret Return type of a function type.
 * @tparam Args Types of arguments of a function type.
 * @tparam Allocator Type of allocator used to manage memory and elements.
 */
template < typename Ret, typename... Args, typename Allocator >
Sink( Signal< Ret( Args... ), Allocator >& ) -> Sink< Signal< Ret( Args... ), Allocator > >;
} // namespace onyx
