#pragma once

#include <thread>

namespace onyx::threading {
/**
 * @brief The ThreadPoolOptions class provides creation options for
 * ThreadPool.
 */
class ThreadPoolOptions {
  public:
    /**
     * @brief ThreadPoolOptions Construct default options for thread pool.
     */
    ThreadPoolOptions();
    explicit ThreadPoolOptions( int32_t threadCount );

    /**
     * @brief setThreadCount Set thread count.
     * @param count Number of threads to be created.
     */
    void setThreadCount( int32_t count );

    /**
     * @brief setQueueSize Set single worker queue size.
     * @param count Maximum length of queue of single worker.
     */
    void setQueueSize( int32_t size );

    /**
     * @brief threadCount Return thread count.
     */
    ONYX_NO_DISCARD int32_t getThreadCount() const;

    /**
     * @brief queueSize Return single worker queue size.
     */
    ONYX_NO_DISCARD int32_t getQueueSize() const;

  private:
    int32_t m_threadCount;
    int32_t m_queueSize;
};

/// Implementation
inline ThreadPoolOptions::ThreadPoolOptions()
    : ThreadPoolOptions( std::max< int32_t >( 1, static_cast< int32_t >( std::thread::hardware_concurrency() ) ) ) {}

inline ThreadPoolOptions::ThreadPoolOptions( int32_t threadCount )
    : m_threadCount( threadCount )
    , m_queueSize( 1024 ) {}

inline void ThreadPoolOptions::setThreadCount( int32_t count ) {
    m_threadCount = std::max< int32_t >( 1, count );
}

inline void ThreadPoolOptions::setQueueSize( int32_t size ) {
    m_queueSize = std::max< int32_t >( 1, size );
}

inline int32_t ThreadPoolOptions::getThreadCount() const {
    return m_threadCount;
}

inline int32_t ThreadPoolOptions::getQueueSize() const {
    return m_queueSize;
}
} // namespace onyx::threading
