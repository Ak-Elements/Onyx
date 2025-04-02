#pragma once

#include <thread>

namespace Onyx
{
    namespace Threading
    {
        /**
         * @brief The ThreadPoolOptions class provides creation options for
         * ThreadPool.
         */
        class ThreadPoolOptions
        {
        public:
            /**
             * @brief ThreadPoolOptions Construct default options for thread pool.
             */
            ThreadPoolOptions();
            explicit ThreadPoolOptions(onyxS32 threadCount);

            /**
             * @brief setThreadCount Set thread count.
             * @param count Number of threads to be created.
             */
            void SetThreadCount(onyxS32 count);

            /**
             * @brief setQueueSize Set single worker queue size.
             * @param count Maximum length of queue of single worker.
             */
            void SetQueueSize(onyxS32 size);

            /**
             * @brief threadCount Return thread count.
             */
            onyxS32 GetThreadCount() const;

            /**
             * @brief queueSize Return single worker queue size.
             */
            onyxS32 GetQueueSize() const;

        private:
            onyxS32 m_ThreadCount;
            onyxS32 m_QueueSize;
        };

        /// Implementation
        inline ThreadPoolOptions::ThreadPoolOptions()
            : ThreadPoolOptions(std::max<onyxS32>(1, static_cast<onyxS32>(std::thread::hardware_concurrency())))
        {
        }

        inline ThreadPoolOptions::ThreadPoolOptions(onyxS32 threadCount)
            : m_ThreadCount(threadCount)
            , m_QueueSize(1024)
        {
        }

        inline void ThreadPoolOptions::SetThreadCount(onyxS32 count)
        {
            m_ThreadCount = std::max<onyxS32>(1, count);
        }

        inline void ThreadPoolOptions::SetQueueSize(onyxS32 size)
        {
            m_QueueSize = std::max<onyxS32>(1, size);
        }

        inline onyxS32 ThreadPoolOptions::GetThreadCount() const
        {
            return m_ThreadCount;
        }

        inline onyxS32 ThreadPoolOptions::GetQueueSize() const
        {
            return m_QueueSize;
        }
    }
}
