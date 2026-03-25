#pragma once

#include <onyx/inplacefunction.h>
#include <onyx/onyx_types.h>

#include <future>
#include <stop_token>
#include <utility>
#include <variant>

#define REQUIRES( ... ) typename std::enable_if< ( __VA_ARGS__ ) >::type* = nullptr

namespace onyx::threading {

template < typename Signature, size_t Capacity >
class AsyncTask; // forward declare

namespace details {
template < typename T >
struct ReturnValueType {
    using Type = T;
};

template <>
struct ReturnValueType< void > {
    using Type = std::monostate;
};

template < typename T >
class FutureSharedState {
  public:
    FutureSharedState() = default;
    explicit FutureSharedState( std::stop_source stopSource )
        : m_stopSource( std::move( stopSource ) ) {}

  private:
    enum class State : uint8_t {
        Pending,
        Completed,
        Cancelled,
        Failure,
    };

  public:
    void wait() const {
        if ( m_state != State::Pending )
            return;

        std::atomic_wait( &m_state, State::Pending );
    }

    T get() const {
        if ( isPending() ) {
            wait();
        }

        if constexpr ( std::is_same_v< T, void > == false ) {
            if ( m_state.load( std::memory_order::relaxed ) == State::Completed ) {
                return std::get< 1 >( m_value );
            }

            return T{};
        }
    }

    bool isPending() const { return m_state.load( std::memory_order::relaxed ) == State::Pending; }
    bool isCompleted() const { return m_state.load( std::memory_order::relaxed ) == State::Completed; }
    bool isCancelled() const { return m_state.load( std::memory_order::relaxed ) == State::Cancelled; }

    template < typename U = T, REQUIRES( std::is_void< U >::value ) >
    void setValue() {
        if ( updateState( State::Completed ) ) {
            if ( m_continuation != nullptr ) {
                m_continuation();
            }

            std::atomic_notify_all( &m_state );
        }
    }

    template < typename U = T, REQUIRES( !std::is_void< U >::value ) >
    void setValue( U&& val ) {
        if ( updateState( State::Completed ) ) {
            m_value = std::forward< U >( val );

            if ( m_continuation != nullptr ) {
                m_continuation();
            }

            std::atomic_notify_all( &m_state );
        }
    }

    void cancel() {
        if ( updateState( State::Cancelled ) ) {
            m_stopSource.request_stop();
            if ( m_cancelCallback != nullptr ) {
                m_cancelCallback();
            }

            std::atomic_notify_all( &m_state );
        }
    }

    void cancel( bool waitForCancel ) {
        cancel();

        if ( waitForCancel ) {
            wait();
        }
    }

    template < typename Callable >
    void then( Callable&& c ) {
        m_continuation = std::move( c );
    }

    template < typename Callable >
    void onCancel( Callable&& c ) {
        m_cancelCallback = std::move( c );
    }

    std::stop_token getStopToken() { return m_stopSource.get_token(); }

  private:
    bool updateState( State desired ) {
        State currentState = m_state.load( std::memory_order::relaxed );
        do {
            if ( ( currentState == desired ) || ( currentState == State::Failure ) ||
                 ( currentState == State::Cancelled ) ) {
                return false;
            }

        } while ( m_state.compare_exchange_weak( currentState, desired ) == false );

        return true;
    }

  private:
    Atomic< State > m_state{ State::Pending };

    std::variant< std::monostate, typename ReturnValueType< T >::Type, std::exception_ptr > m_value;
    std::stop_source m_stopSource;

    InplaceFunction< void() > m_continuation = nullptr;
    InplaceFunction< void() > m_cancelCallback = nullptr;
};
} // namespace details

template < typename T >
class Future {
  public:
    Future()
        : m_futureState( nullptr ) {}

    explicit Future( SharedPtr< details::FutureSharedState< T > > futureState )
        : m_futureState( futureState ) {}

    Future( const Future& other )
        : m_futureState( other.m_futureState ) {}

    Future( Future&& other ) noexcept
        : m_futureState( std::move( other.m_futureState ) ) {}

    Future& operator=( const Future& other ) {
        if ( this == &other )
            return *this;

        m_futureState = other.m_futureState;
        return *this;
    }

    Future& operator=( Future&& other ) noexcept {
        if ( this == &other )
            return *this;

        m_futureState = std::move( other.m_futureState );
        return *this;
    }

    explicit operator bool() const { return ( m_futureState != nullptr ) && ( isPending() == false ); }

    T get() {
        ONYX_ASSERT( m_FutureState != nullptr );
        return m_futureState->get();
    }

    void wait() {
        ONYX_ASSERT( m_FutureState != nullptr );
        m_futureState->wait();
    }

    ONYX_NO_DISCARD bool isPending() const {
        ONYX_ASSERT( m_FutureState != nullptr );
        return m_futureState->isPending();
    }

    ONYX_NO_DISCARD bool isCompleted() const {
        ONYX_ASSERT( m_FutureState != nullptr );
        return m_futureState->isCompleted();
    }

    ONYX_NO_DISCARD bool isCancelled() const {
        ONYX_ASSERT( m_FutureState != nullptr );
        return m_futureState->isCancelled();
    }

    void cancel() {
        ONYX_ASSERT( m_FutureState != nullptr );
        m_futureState->cancel();
    }

    void cancel( bool waitForCancel ) {
        ONYX_ASSERT( m_FutureState != nullptr );
        m_futureState->cancel();

        if ( waitForCancel )
            m_futureState->wait();
    }

    template < typename Callable >
    void then( Callable&& c ) {
        ONYX_ASSERT( m_FutureState != nullptr );
        m_futureState->then( std::forward< Callable >( c ) );
    }

    template < typename Callable >
    void onCancel( Callable&& c ) {
        ONYX_ASSERT( m_FutureState != nullptr );
        m_futureState->onCancel( std::forward< Callable >( c ) );
    }

  private:
    SharedPtr< details::FutureSharedState< T > > m_futureState;
};

template < typename T >
class Promise {
  public:
    Promise()
        : m_futureState( std::make_shared< details::FutureSharedState< T > >() ) {}

    // TODO: fix to remove copy ctor
    Promise( const Promise& other ) = default;
    Promise& operator=( const Promise& other ) = default;

    Promise( Promise&& other ) noexcept
        : m_futureState( std::move( other.m_futureState ) ) {}

    Promise& operator=( Promise&& other ) noexcept {
        if ( *this == other )
            return *this;

        std::swap( m_futureState, other.m_futureState );
    }

    Future< T > getFuture() const { return Future< T >{ m_futureState }; }

    template < typename U = T, REQUIRES( std::is_void< U >::value ) >
    void setValue() {
        m_futureState->setValue();
    }

    template < typename U = T, REQUIRES( !std::is_void< U >::value ) >
    void setValue( U&& val ) {
        m_futureState->setValue( std::forward< U >( val ) );
    }

    std::stop_token getStopToken() { return m_futureState->getStopToken(); }

  private:
    SharedPtr< details::FutureSharedState< T > > m_futureState;
};
} // namespace onyx::threading

#undef REQUIRES