#pragma once

#include <onyx/thread/threadpool/threadpooloptions.h>
#include <onyx/thread/threadpool/worker.h>
#include <onyx/thread/container/lockfreempmcboundedqueue.h>

#include <onyx/inplacefunction.h>
#include <onyx/log/logger.h>

namespace Onyx::Threading
{
   /* namespace Internal
    {
        template<typename Signature, size_t Capacity>
        class AsyncTask;
    }*/

    template <typename Task, template<typename> class Queue>
    class ThreadPoolImpl;
    using ThreadPool = ThreadPoolImpl<InplaceFunction<void(), 128>, LockFreeMPMCBoundedQueue>;

    template <typename Task, template<typename> class Queue>
    class ThreadPoolImpl
    {
    public:
        explicit ThreadPoolImpl(const ThreadPoolOptions& options = ThreadPoolOptions());
#if ONYX_PROFILER_ENABLED
        explicit ThreadPoolImpl(const ThreadPoolOptions& options, const char* profilerName);
#endif

        ThreadPoolImpl(ThreadPoolImpl&& rhs) noexcept;
        ~ThreadPoolImpl();

        ThreadPoolImpl& operator=(ThreadPoolImpl&& rhs) noexcept;

        //template <typename Callable>
        //auto Emplace(Callable&& functor) -> Future<std::invoke_result_t<Callable>>
        //{
        //    using ReturnType = std::invoke_result_t<Callable>;

        //    // TODO remove async task requirement
        //    AsyncTask<Callable, 32> task(std::forward<Callable>(functor), m_FutureState->GetStopToken());
        //    Future<ReturnType> future = task.GetFuture();
        //    Post(std::move(task));
        //    return future;
        //}

        template <typename Handler>
        bool TryPost(Handler&& handler);

        template <typename Handler>
        void Post(Handler&& handler);

    private:
        Worker<Task, Queue>& GetWorker();

        DynamicArray<UniquePtr<Worker<Task, Queue>>> m_Workers;
        Atomic<onyxU32> m_NextWorker;
        std::stop_source m_StopSource;
        std::condition_variable m_WaitForTasks;
    };

    /// Implementation
#if ONYX_PROFILER_ENABLED 
    template <typename Task, template<typename> class Queue>
    inline ThreadPoolImpl<Task, Queue>::ThreadPoolImpl(const ThreadPoolOptions& options, const char* profilerName)
        : m_Workers(options.GetThreadCount())
        , m_NextWorker(0)
    {
        for (UniquePtr<Worker<Task, Queue>>& workerPtr : m_Workers)
        {
            workerPtr.reset(new Worker<Task, Queue>(options.GetQueueSize(), m_WaitForTasks));
        }

        for (onyxU32 i = 0; i < m_Workers.size(); ++i)
        {
            Worker<Task, Queue>* stealDonor = m_Workers[(i + 1) % m_Workers.size()].get();

            m_Workers[i]->Start(i, stealDonor, m_StopSource.get_token(), Format::Format("{}_{}", profilerName, i));
        }
    }
#endif

    template <typename Task, template<typename> class Queue>
    inline ThreadPoolImpl<Task, Queue>::ThreadPoolImpl(
        const ThreadPoolOptions& options)
        : m_Workers(options.GetThreadCount())
        , m_NextWorker(0)
    {
        for (UniquePtr<Worker<Task, Queue>>& workerPtr : m_Workers)
        {
            workerPtr.reset(new Worker<Task, Queue>(options.GetQueueSize(), m_WaitForTasks));
        }

        for (onyxU32 i = 0; i < m_Workers.size(); ++i)
        {
            Worker<Task, Queue>* stealDonor = m_Workers[(i + 1) % m_Workers.size()].get();
#if ONYX_PROFILER_ENABLED
            m_Workers[i]->Start(i, stealDonor, m_StopSource.get_token(), "");
#else
            m_Workers[i]->Start(i, stealDonor, m_StopSource.get_token());
#endif
        }
    }

    template <typename Task, template<typename> class Queue>
    inline ThreadPoolImpl<Task, Queue>::ThreadPoolImpl(ThreadPoolImpl<Task, Queue>&& rhs) noexcept
    {
        *this = rhs;
    }

    template <typename Task, template<typename> class Queue>
    inline ThreadPoolImpl<Task, Queue>::~ThreadPoolImpl()
    {
        m_StopSource.request_stop();
        m_WaitForTasks.notify_all();
        // wait for join
    }

    template <typename Task, template<typename> class Queue>
    inline ThreadPoolImpl<Task, Queue>&
        ThreadPoolImpl<Task, Queue>::operator=(ThreadPoolImpl<Task, Queue>&& rhs) noexcept
    {
        if (this != &rhs)
        {
            m_Workers = std::move(rhs.m_Workers);
            m_NextWorker = rhs.m_NextWorker.load();
        }
        return *this;
    }

    template <typename Task, template<typename> class Queue>
    template <typename Handler>
    inline bool ThreadPoolImpl<Task, Queue>::TryPost(Handler&& handler)
    {
        bool success = GetWorker().Post(std::forward<Handler>(handler));
        m_WaitForTasks.notify_all();
        return success;
    }

    template <typename Task, template<typename> class Queue>
    template <typename Handler>
    inline void ThreadPoolImpl<Task, Queue>::Post(Handler&& handler)
    {
        const bool success = TryPost(std::forward<Handler>(handler));
        if (!success)
        {
            ONYX_LOG_ERROR("Thread pool queue is full.");
        }
    }

    template <typename Task, template<typename> class Queue>
    inline Worker<Task, Queue>& ThreadPoolImpl<Task, Queue>::GetWorker()
    {
        size_t id = Worker<Task, Queue>::GetWorkerIdForCurrentThread();
        size_t workerCount = m_Workers.size();
        if (id > workerCount)
        {
            id = m_NextWorker.fetch_add(1, std::memory_order_relaxed) % workerCount;
        }

        return *m_Workers[id];
    }

    inline ThreadPool DefaultThreadPool;
}
