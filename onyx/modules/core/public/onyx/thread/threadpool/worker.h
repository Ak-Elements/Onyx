#pragma once

#include <onyx/profiler/profiler.h>

#include <condition_variable>
#include <stop_token>

namespace onyx::threading {
/**
 * @brief The Worker class owns task queue and executing thread.
 * In thread it tries to pop task from queue. If queue is empty then it tries
 * to steal task from the sibling worker. If steal was unsuccessful then spins
 * with one millisecond delay.
 */
template < typename Task, template < typename > class Queue >
class Worker {
  public:
    /**
     * @brief Worker Constructor.
     * @param queue_size Length of underlying task queue.
     */
    explicit Worker( int32_t queueSize, std::condition_variable& waitForWork );

    /**
     * @brief Move ctor implementation.
     */
    Worker( Worker&& rhs ) noexcept;

    ~Worker() noexcept;

    /**
     * @brief Move assignment implementation.
     */
    Worker& operator=( Worker&& rhs ) noexcept;

    /**
     * @brief start Create the executing thread and start tasks execution.
     * @param id Worker ID.
     * @param steal_donor Sibling worker to steal task from it.
     */
#if ONYX_PROFILER_ENABLED
    void start( int64_t id, Worker* stealDonor, std::stop_token token, StringView profilerName );
#else
    void start( int64_t id, Worker* stealDonor, std::stop_token token );
#endif
    /**
     * @brief stop Stop all worker's thread and stealing activity.
     * Waits until the executing thread became finished.
     */
    void stop();

    /**
     * @brief post Post task to queue.
     * @param handler Handler to be executed in executing thread.
     * @return true on success.
     */
    template < typename Handler >
    bool post( Handler&& handler );

    /**
     * @brief steal Steal one task from this worker queue.
     * @param task Place for stealed task to be stored.
     * @return true on success.
     */
    bool steal( Task& task );

    /**
     * @brief getWorkerIdForCurrentThread Return worker ID associated with
     * current thread if exists.
     * @return Worker ID.
     */
    static size_t getWorkerIdForCurrentThread();

  private:
    /**
     * @brief doWork Executing thread function.
     * @param id Worker ID to be associated with this thread.
     * @param steal_donor Sibling worker to steal task from it.
     */
    void doWork( int64_t id, Worker* stealDonor );

    Queue< Task > m_queue;
    std::thread m_thread;
    std::stop_token m_stopToken;
    std::condition_variable* m_waitForWork{};

#if ONYX_PROFILER_ENABLED
    String m_ProfilerName;
#endif
};

/// Implementation
namespace detail {
inline int64_t* threadId() {
    static thread_local int64_t TssId = -1;
    return &TssId;
}
} // namespace detail

template < typename Task, template < typename > class Queue >
inline Worker< Task, Queue >::Worker( int32_t queueSize, std::condition_variable& waitForWork )
    : m_queue( queueSize )
    , m_waitForWork( &waitForWork ) {}

template < typename Task, template < typename > class Queue >
inline Worker< Task, Queue >::Worker( Worker&& rhs ) noexcept {
    *this = rhs;
}

template < typename Task, template < typename > class Queue >
inline Worker< Task, Queue >& Worker< Task, Queue >::operator=( Worker&& rhs ) noexcept {
    if ( this != &rhs ) {
        m_queue = std::move( rhs.m_queue );
        m_stopToken = std::move( rhs.m_stopToken );
        m_thread = std::move( rhs.m_thread );
    }
    return *this;
}

template < typename Task, template < typename > class Queue >
inline Worker< Task, Queue >::~Worker() noexcept {
    m_thread.join();
}

template < typename Task, template < typename > class Queue >
#if ONYX_PROFILER_ENABLED
inline void Worker< Task, Queue >::Start( int64_t id,
                                          Worker* stealDonor,
                                          std::stop_token token,
                                          StringView profilerName )
#else
inline void Worker< Task, Queue >::start( int64_t id, Worker* stealDonor, std::stop_token token )
#endif
{
#if ONYX_PROFILER_ENABLED
    m_ProfilerName = String( profilerName );
#endif
    m_stopToken = std::move( token );
    m_thread = std::thread( &Worker< Task, Queue >::doWork, this, id, stealDonor );
}

template < typename Task, template < typename > class Queue >
inline size_t Worker< Task, Queue >::getWorkerIdForCurrentThread() {
    return *detail::threadId();
}

template < typename Task, template < typename > class Queue >
template < typename Handler >
inline bool Worker< Task, Queue >::post( Handler&& handler ) {
    return m_queue.push( std::forward< Handler >( handler ) );
}

template < typename Task, template < typename > class Queue >
inline bool Worker< Task, Queue >::steal( Task& task ) {
    return m_queue.pop( task );
}

template < typename Task, template < typename > class Queue >
inline void Worker< Task, Queue >::doWork( int64_t id, Worker* stealDonor ) {
    *detail::threadId() = id;
#if ONYX_PROFILER_ENABLED
    if ( m_ProfilerName.empty() == false ) {
        tracy::SetThreadName( m_ProfilerName.c_str() );
    }
#endif

    Task handler;

    while ( m_stopToken.stop_requested() == false ) {
        if ( m_queue.pop( handler ) || stealDonor->steal( handler ) ) {
            // try
            {
                handler();
            }
            // catch (...)
            //{
            //  suppress all exceptions
            //}
        } else {
            std::mutex waitMutex;
            std::unique_lock lock( waitMutex );
            m_waitForWork->wait( lock );
        }
    }
}
} // namespace onyx::threading
