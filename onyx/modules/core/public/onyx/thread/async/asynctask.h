#pragma once

#include <onyx/inplacefunction.h>
#include <onyx/thread/async/future.h>
#include <stop_token>

namespace onyx::threading {
template < typename Signature, size_t Capacity = inplace_function_detail::InplaceFunctionDefaultCapacity >
class AsyncTask; // unspecified

template < typename R, typename... Args, size_t Capacity >
class AsyncTask< R( Args... ), Capacity > {
  private:
    template < typename T, typename U >
    using IsEnabled = std::enable_if_t< !( std::is_same< T, U >::value || std::is_convertible< T, U >::value ) >;

  public:
    using ReturnT = R;
    using FutureT = Future< R >;
    using TaskT = InplaceFunction< R( Args... ), Capacity >;
    template < typename, size_t > friend class AsyncTask;

    AsyncTask() = default;
    explicit AsyncTask( const TaskT& taskFunctor )
        : m_taskFunctor( taskFunctor ) {
        m_stopToken = m_promise.getStopToken();
    }

    AsyncTask( const TaskT& taskFunctor, std::stop_token&& stopToken )
        : m_stopToken( std::move( stopToken ) )
        , m_taskFunctor( taskFunctor ) {}

    // TODO: fix to remove copy ctor - needs promise to not be copyable
    AsyncTask( const AsyncTask& other ) = default;
    AsyncTask& operator=( const AsyncTask& other ) = default;

    AsyncTask( AsyncTask&& other ) noexcept
        : m_taskFunctor( std::move( other.m_taskFunctor ) )
        , m_promise( std::move( other.m_promise ) )
        , m_stopToken( std::move( other.m_stopToken ) ) {}

    AsyncTask& operator=( AsyncTask&& other ) noexcept {
        if ( *this == other ) {
            return *this;
        }

        std::swap( m_taskFunctor, other.m_taskFunctor );
        std::swap( m_stopToken, other.m_stopToken );
        std::swap( m_promise, other.m_promise );
    }

    ~AsyncTask() = default;

    ONYX_NO_DISCARD bool isCanceled() const { return m_stopToken.stop_requested(); }

    FutureT getFuture() const { return m_promise.getFuture(); }

    void operator()() {
        if constexpr ( std::is_void< ReturnT >::value ) {
            if constexpr ( std::is_invocable_v< TaskT, std::stop_token > ) {
                m_taskFunctor( m_stopToken );
            } else {
                m_taskFunctor();
            }

            m_promise.setValue();
        } else {
            if constexpr ( std::is_invocable_v< TaskT, std::stop_token > ) {
                m_promise.setValue( m_taskFunctor( m_stopToken ) );
            } else {
                m_promise.setValue( m_taskFunctor() );
            }
        }
    }

  private:
    TaskT m_taskFunctor = nullptr;
    Promise< ReturnT > m_promise;
    std::stop_token m_stopToken;
};

} // namespace onyx::threading