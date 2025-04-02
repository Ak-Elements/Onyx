#pragma once

#include <onyx/profiler/profiler.h>
#include <stop_token>

namespace Onyx
{
    namespace Threading
    {
        /**
         * @brief The Worker class owns task queue and executing thread.
         * In thread it tries to pop task from queue. If queue is empty then it tries
         * to steal task from the sibling worker. If steal was unsuccessful then spins
         * with one millisecond delay.
         */
        template <typename Task, template<typename> class Queue>
        class Worker
        {
        public:
            /**
             * @brief Worker Constructor.
             * @param queue_size Length of underlying task queue.
             */
            explicit Worker(onyxS32 queue_size, std::condition_variable& waitForWork);

            /**
             * @brief Move ctor implementation.
             */
            Worker(Worker&& rhs) noexcept;

            ~Worker() noexcept;

            /**
             * @brief Move assignment implementation.
             */
            Worker& operator=(Worker&& rhs) noexcept;

            /**
             * @brief start Create the executing thread and start tasks execution.
             * @param id Worker ID.
             * @param steal_donor Sibling worker to steal task from it.
             */
#if ONYX_PROFILER_ENABLED
            void Start(onyxS64 id, Worker* steal_donor, std::stop_token token, StringView profilerName);
#else
            void Start(onyxS64 id, Worker* steal_donor, std::stop_token token);
#endif
            /**
             * @brief stop Stop all worker's thread and stealing activity.
             * Waits until the executing thread became finished.
             */
            void Stop();

            /**
             * @brief post Post task to queue.
             * @param handler Handler to be executed in executing thread.
             * @return true on success.
             */
            template <typename Handler>
            bool Post(Handler&& handler);

            /**
             * @brief steal Steal one task from this worker queue.
             * @param task Place for stealed task to be stored.
             * @return true on success.
             */
            bool Steal(Task& task);

            /**
             * @brief getWorkerIdForCurrentThread Return worker ID associated with
             * current thread if exists.
             * @return Worker ID.
             */
            static size_t GetWorkerIdForCurrentThread();

        private:
            /**
             * @brief doWork Executing thread function.
             * @param id Worker ID to be associated with this thread.
             * @param steal_donor Sibling worker to steal task from it.
             */
            void doWork(onyxS64 id, Worker* steal_donor);

            Queue<Task> m_Queue;
            std::thread m_Thread;
            std::stop_token m_StopToken;
            std::condition_variable* m_WaitForWork;

#if ONYX_PROFILER_ENABLED
            String m_ProfilerName;
#endif

        };

        /// Implementation
        namespace detail
        {
            inline onyxS64* thread_id()
            {
                static thread_local onyxS64 tss_id = -1;
                return &tss_id;
            }
        }

        template <typename Task, template<typename> class Queue>
        inline Worker<Task, Queue>::Worker(onyxS32 queueSize, std::condition_variable& waitForWork)
            : m_Queue(queueSize)
            , m_WaitForWork(&waitForWork)
        {
        }

        template <typename Task, template<typename> class Queue>
        inline Worker<Task, Queue>::Worker(Worker&& rhs) noexcept
        {
            *this = rhs;
        }

        template <typename Task, template<typename> class Queue>
        inline Worker<Task, Queue>& Worker<Task, Queue>::operator=(Worker&& rhs) noexcept
        {
            if (this != &rhs)
            {
                m_Queue = std::move(rhs.m_Queue);
                m_StopToken = std::move(rhs.m_StopToken);
                m_Thread = std::move(rhs.m_Thread);
            }
            return *this;
        }

        template <typename Task, template<typename> class Queue>
        inline Worker<Task, Queue>::~Worker() noexcept
        {
            m_Thread.join();
        }


        template <typename Task, template<typename> class Queue>
#if ONYX_PROFILER_ENABLED
        inline void Worker<Task, Queue>::Start(onyxS64 id, Worker* steal_donor, std::stop_token token, StringView profilerName)
#else
        inline void Worker<Task, Queue>::Start(onyxS64 id, Worker* steal_donor, std::stop_token token)
#endif
        {
#if ONYX_PROFILER_ENABLED
            m_ProfilerName = String(profilerName);
#endif
            m_StopToken = std::move(token);
            m_Thread = std::thread(&Worker<Task, Queue>::doWork, this, id, steal_donor);
        }



        template <typename Task, template<typename> class Queue>
        inline size_t Worker<Task, Queue>::GetWorkerIdForCurrentThread()
        {
            return *detail::thread_id();
        }

        template <typename Task, template<typename> class Queue>
        template <typename Handler>
        inline bool Worker<Task, Queue>::Post(Handler&& handler)
        {
            return m_Queue.Push(std::forward<Handler>(handler));
        }

        template <typename Task, template<typename> class Queue>
        inline bool Worker<Task, Queue>::Steal(Task& task)
        {
            return m_Queue.Pop(task);
        }

        template <typename Task, template<typename> class Queue>
        inline void Worker<Task, Queue>::doWork(onyxS64 id, Worker* steal_donor)
        {
            *detail::thread_id() = id;
#if ONYX_PROFILER_ENABLED
            if (m_ProfilerName.empty() == false)
            {
                tracy::SetThreadName(m_ProfilerName.c_str());
            }
#endif

            Task handler;

            while (m_StopToken.stop_requested() == false)
            {
                if (m_Queue.Pop(handler) || steal_donor->Steal(handler))
                {
                    //try
                    {
                        handler();
                    }
                    //catch (...)
                    //{
                        // suppress all exceptions
                    //}
                }
                else
                {
                    std::mutex waitMutex;
                    std::unique_lock lock(waitMutex);
                    m_WaitForWork->wait(lock);
                }
            }
        }
    }
}
