#pragma once

#include <onyx/inplacefunction.h>
#include <onyx/thread/async/future.h>
#include <onyx/thread/synchronization/stoptoken.h>

namespace Onyx::Threading
{
template<typename Signature, size_t Capacity = InplaceFunctionDetail::InplaceFunctionDefaultCapacity>
class AsyncTask; // unspecified

class AsyncTaskGroup
{
private:
    template <typename T, typename U>
    using is_enabled = std::enable_if_t <
        !(std::is_same<T, U>::value
            || std::is_convertible<T, U>::value)>;

public:
    using ReturnT = R;
    using FutureT = Future<R>;
    using TaskT = InplaceFunction<R(Args...), Capacity>;
    template <typename, size_t>	friend class AsyncTask;

    AsyncTask() = default;
    AsyncTask(const TaskT& taskFunctor)
        : m_TaskFunctor(taskFunctor)
    {
        m_StopToken = m_Promise.GetStopToken();
    }

    AsyncTask(const TaskT& taskFunctor, std::stop_token&& stopToken)
        : m_StopToken(std::move(stopToken))
        , m_TaskFunctor(taskFunctor)
    {
    }

    // TODO: fix to remove copy ctor - needs promise to not be copyable
    AsyncTask(const AsyncTask& other) = default;
    AsyncTask& operator=(const AsyncTask& other) = default;

    AsyncTask(AsyncTask&& other) noexcept = default;
    AsyncTask& operator=(AsyncTask&& other) noexcept = default;

    ~AsyncTask() = default;

    bool IsCanceled() const
    {
        return m_StopToken.stop_requested();
    }

    FutureT GetFuture() const { return m_Promise.GetFuture(); }

    void operator()()
    {
        if constexpr (std::is_void<ReturnT>::value)
        {
            if constexpr (std::is_invocable_v<TaskT, std::stop_token>)
            {
                m_TaskFunctor(m_StopToken);
            }
            else
            {
                m_TaskFunctor();
            }

            m_Promise.SetValue();
        }
        else
        {
            if constexpr (std::is_invocable_v<TaskT, std::stop_token>)
            {
                m_Promise.SetValue(m_TaskFunctor(m_StopToken));
            }
            else
            {
                m_Promise.SetValue(m_TaskFunctor());
            }
        }
    }

private:
    std::stop_token m_StopToken;
    Promise<void> m_Promise;
    DynamicArray<TaskT> m_TaskFunctor = nullptr;
};

} // namespace Onyx::Threading