#pragma once

#include <condition_variable>
#include <onyx/thread/container/lockfreempmcboundedqueue.h>
#include <onyx/thread/threadpool/threadpooloptions.h>
#include <onyx/thread/threadpool/worker.h>

#include <onyx/inplacefunction.h>
#include <onyx/log/logger.h>

namespace onyx::threading {
/* namespace Internal
 {
     template<typename Signature, size_t Capacity>
     class AsyncTask;
 }*/

template < typename Task, template < typename > class Queue >
class ThreadPoolImpl;
using ThreadPool = ThreadPoolImpl< InplaceFunction< void(), 128 >, LockFreeMPMCBoundedQueue >;

template < typename Task, template < typename > class Queue >
class ThreadPoolImpl {
  public:
    explicit ThreadPoolImpl( const ThreadPoolOptions& options = ThreadPoolOptions() );
#if ONYX_PROFILER_ENABLED
    explicit ThreadPoolImpl( const ThreadPoolOptions& options, const char* profilerName );
#endif

    ThreadPoolImpl( ThreadPoolImpl&& rhs ) noexcept;
    ~ThreadPoolImpl();

    ThreadPoolImpl& operator=( ThreadPoolImpl&& rhs ) noexcept;

    // template <typename Callable>
    // auto Emplace(Callable&& functor) -> Future<std::invoke_result_t<Callable>>
    //{
    //     using ReturnType = std::invoke_result_t<Callable>;

    //    // TODO remove async task requirement
    //    AsyncTask<Callable, 32> task(std::forward<Callable>(functor), m_FutureState->GetStopToken());
    //    Future<ReturnType> future = task.GetFuture();
    //    Post(std::move(task));
    //    return future;
    //}

    template < typename Handler >
    bool tryPost( Handler&& handler );

    template < typename Handler >
    void post( Handler&& handler );

  private:
    Worker< Task, Queue >& getWorker();

    DynamicArray< UniquePtr< Worker< Task, Queue > > > m_workers;
    Atomic< uint32_t > m_nextWorker;
    std::stop_source m_stopSource;
    std::condition_variable m_waitForTasks;
};

/// Implementation
#if ONYX_PROFILER_ENABLED
template < typename Task, template < typename > class Queue >
inline ThreadPoolImpl< Task, Queue >::ThreadPoolImpl( const ThreadPoolOptions& options, const char* profilerName )
    : m_Workers( options.GetThreadCount() )
    , m_NextWorker( 0 ) {
    for ( UniquePtr< Worker< Task, Queue > >& workerPtr : m_Workers ) {
        workerPtr.reset( new Worker< Task, Queue >( options.getQueueSize(), m_WaitForTasks ) );
    }

    for ( uint32_t i = 0; i < m_Workers.size(); ++i ) {
        Worker< Task, Queue >* stealDonor = m_Workers[ ( i + 1 ) % m_Workers.size() ].get();

        m_Workers[ i ]->start( i, stealDonor, m_StopSource.get_token(), format::Format( "{}_{}", profilerName, i ) );
    }
}
#endif

template < typename Task, template < typename > class Queue >
inline ThreadPoolImpl< Task, Queue >::ThreadPoolImpl( const ThreadPoolOptions& options )
    : m_workers( options.getThreadCount() )
    , m_nextWorker( 0 ) {
    for ( UniquePtr< Worker< Task, Queue > >& workerPtr : m_workers ) {
        workerPtr.reset( new Worker< Task, Queue >( options.getQueueSize(), m_waitForTasks ) );
    }

    for ( uint32_t i = 0; i < m_workers.size(); ++i ) {
        Worker< Task, Queue >* stealDonor = m_workers[ ( i + 1 ) % m_workers.size() ].get();
#if ONYX_PROFILER_ENABLED
        m_Workers[ i ]->start( i, stealDonor, m_StopSource.get_token(), "" );
#else
        m_workers[ i ]->start( i, stealDonor, m_stopSource.get_token() );
#endif
    }
}

template < typename Task, template < typename > class Queue >
inline ThreadPoolImpl< Task, Queue >::ThreadPoolImpl( ThreadPoolImpl< Task, Queue >&& rhs ) noexcept {
    *this = rhs;
}

template < typename Task, template < typename > class Queue >
inline ThreadPoolImpl< Task, Queue >::~ThreadPoolImpl() {
    m_stopSource.request_stop();
    m_waitForTasks.notify_all();
    // wait for join
}

template < typename Task, template < typename > class Queue >
inline ThreadPoolImpl< Task, Queue >& ThreadPoolImpl< Task, Queue >::operator=(
    ThreadPoolImpl< Task, Queue >&& rhs ) noexcept {
    if ( this != &rhs ) {
        m_workers = std::move( rhs.m_workers );
        m_nextWorker = rhs.m_nextWorker.load();
    }
    return *this;
}

template < typename Task, template < typename > class Queue >
template < typename Handler >
inline bool ThreadPoolImpl< Task, Queue >::tryPost( Handler&& handler ) {
    bool success = getWorker().post( std::forward< Handler >( handler ) );
    m_waitForTasks.notify_all();
    return success;
}

template < typename Task, template < typename > class Queue >
template < typename Handler >
inline void ThreadPoolImpl< Task, Queue >::post( Handler&& handler ) {
    const bool success = tryPost( std::forward< Handler >( handler ) );
    if ( !success ) {
        ONYX_LOG_ERROR( "Thread pool queue is full." );
    }
}

template < typename Task, template < typename > class Queue >
inline Worker< Task, Queue >& ThreadPoolImpl< Task, Queue >::getWorker() {
    size_t id = Worker< Task, Queue >::getWorkerIdForCurrentThread();
    size_t workerCount = m_workers.size();
    if ( id > workerCount ) {
        id = m_nextWorker.fetch_add( 1, std::memory_order::relaxed ) % workerCount;
    }

    return *m_workers[ id ];
}

inline ThreadPool g_defaultThreadPool;
} // namespace onyx::threading
