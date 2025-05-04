#pragma once

#include <onyx/inplacefunction.h>
#include <onyx/thread/async/future.h>
#include <stop_token>

namespace Onyx::Threading
{
template<typename Signature, size_t Capacity = InplaceFunctionDetail::InplaceFunctionDefaultCapacity>
class AsyncTask; // unspecified

template <typename R, typename... Args, size_t Capacity>
class AsyncTask<R(Args...), Capacity>
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
    explicit AsyncTask(const TaskT& taskFunctor)
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

    AsyncTask(AsyncTask&& other) noexcept
        : m_TaskFunctor(std::move(other.m_TaskFunctor))
        , m_StopToken(std::move(other.m_StopToken))
        , m_Promise(std::move(other.m_Promise))
    {
    }

    AsyncTask& operator=(AsyncTask&& other)
    {
        if (*this == other)
        {
            return *this;
        }

        std::swap(m_TaskFunctor, other.m_TaskFunctor);
        std::swap(m_StopToken, other.m_StopToken);
        std::swap(m_Promise, other.m_Promise);
    }

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
    TaskT m_TaskFunctor = nullptr;
    Promise<ReturnT> m_Promise;
    std::stop_token m_StopToken;
};

} // namespace Onyx::Threading