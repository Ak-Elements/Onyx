#pragma once

#include <onyx/inplacefunction.h>
#include <onyx/onyx_types.h>

#include <variant>
#include <utility>
#include <future>
#include <stop_token>

#define REQUIRES(...) typename std::enable_if<(__VA_ARGS__)>::type* = nullptr

namespace Onyx::Threading
{

template<typename Signature, size_t Capacity>
class AsyncTask; // forward declare

namespace details
{
    template <typename T>
    struct return_value_type
    {
        using type = T;
    };

    template <>
    struct return_value_type<void>
    {
        using type = std::monostate;
    };

    template <typename T>
    class future_shared_state
    {
    public:
        future_shared_state() = default;
        explicit future_shared_state(const std::stop_source& m_stop_source)
            : m_StopSource(m_stop_source)
        {
        }

    private:
        enum class State : onyxU8
        {
            Pending,
            Completed,
            Cancelled,
            Failure,
        };

    public:

        void Wait() const
        {
            if (m_State != State::Pending)
                return;

            std::atomic_wait(&m_State, State::Pending);
        }

        T Get() const
        {
            if (IsPending())
            {
                Wait();
            }

            if constexpr (std::is_same_v<T, void> == false)
            {
                if (m_State.load(std::memory_order_relaxed) == State::Completed)
                {
                    return std::get<1>(m_Value);
                }

                return T{};
            }
        }

        bool IsPending() const
        {
            return m_State.load(std::memory_order_relaxed) == State::Pending;
        }

        bool IsCompleted() const
        {
            return m_State.load(std::memory_order_relaxed) == State::Completed;
        }

        bool IsCancelled() const
        {
            return m_State.load(std::memory_order_relaxed) == State::Cancelled;
        }

        template <typename U = T, REQUIRES(std::is_void<U>::value)>
        void SetValue()
        {
            if (UpdateState(State::Completed))
            {
                if (m_Continuation != nullptr)
                {
                    m_Continuation();
                }

                std::atomic_notify_all(&m_State);
            }
        }

        template <typename U = T, REQUIRES(!std::is_void<U>::value)>
        void SetValue(U&& val)
        {
            if (UpdateState(State::Completed))
            {
                m_Value = std::forward<U>(val);

                if (m_Continuation != nullptr)
                {
                    m_Continuation();
                }

                std::atomic_notify_all(&m_State);
            }
        }

        void Cancel()
        {
            if (UpdateState(State::Cancelled))
            {
                m_StopSource.request_stop();
                if (m_CancelCallback != nullptr)
                {
                    m_CancelCallback();
                }

                std::atomic_notify_all(&m_State);
            }
            
        }

        void Cancel(bool waitForCancel)
        {
            Cancel();

            if (waitForCancel)
            {
                Wait();
            }
        }

        template <typename Callable>
        void Then(Callable&& c)
        {
            m_Continuation = std::move(c);
        }

        template <typename Callable>
        void OnCancel(Callable&& c)
        {
            m_CancelCallback = std::move(c);
        }

        std::stop_token GetStopToken() { return m_StopSource.get_token(); }

    private:

        bool UpdateState(State desired)
        {
            State currentState = m_State.load(std::memory_order_relaxed);
            do
            {
                if ((currentState == desired) ||
                    (currentState == State::Failure) ||
                    (currentState == State::Cancelled))
                {
                    return false;
                }

            } while (m_State.compare_exchange_weak(currentState, desired) == false);

            return true;
        }
    private:
        Atomic<State> m_State { State::Pending };

        std::variant<std::monostate, typename return_value_type<T>::type, std::exception_ptr> m_Value;
        std::stop_source m_StopSource;

        InplaceFunction<void()> m_Continuation = nullptr;
        InplaceFunction<void()> m_CancelCallback = nullptr;
    };
}

template <typename T>
class Future
{
public:
    Future()
        : m_FutureState(nullptr)
    {
    }

    explicit Future(SharedPtr<details::future_shared_state<T>> futureState)
        : m_FutureState(futureState)
    {
    }

    Future(const Future& other)
        : m_FutureState(other.m_FutureState)
    {
    }

    Future(Future&& other) noexcept
        : m_FutureState(std::move(other.m_FutureState))
    {
    }

    Future& operator=(const Future& other)
    {
        if (this == &other)
            return *this;

        m_FutureState = other.m_FutureState;
        return *this;
    }

    Future& operator=(Future&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_FutureState = std::move(other.m_FutureState);
        return *this;
    }

    operator bool() const { return (m_FutureState != nullptr) && (IsPending() == false); }

    T Get()
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        return m_FutureState->Get();
    }

    void Wait()
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        m_FutureState->Wait();
    }

    bool IsPending() const { ONYX_ASSERT(m_FutureState != nullptr); return m_FutureState->IsPending(); }
    bool IsCompleted() const { ONYX_ASSERT(m_FutureState != nullptr); return m_FutureState->IsCompleted(); }
    bool IsCancelled() const { ONYX_ASSERT(m_FutureState != nullptr); return m_FutureState->IsCancelled(); }

    void Cancel()
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        m_FutureState->Cancel();
    }

    void Cancel(bool waitForCancel)
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        m_FutureState->Cancel();

        if (waitForCancel)
            m_FutureState->Wait();
    }

    template <typename Callable>
    void Then(Callable&& c)
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        m_FutureState->Then(std::forward<Callable>(c));
    }

    template <typename Callable>
    void OnCancel(Callable&& c)
    {
        ONYX_ASSERT(m_FutureState != nullptr);
        m_FutureState->OnCancel(std::forward<Callable>(c));
    }

private:
    SharedPtr<details::future_shared_state<T>> m_FutureState;
};

template <typename T>
class Promise
{
public:
    Promise()
        : m_FutureState(std::make_shared<details::future_shared_state<T>>())
    {
    }

    // TODO: fix to remove copy ctor
    Promise(const Promise& other) = default;
    Promise& operator=(const Promise& other) = default;

    Promise(Promise&& other) noexcept = default;
    Promise& operator=(Promise&& other) noexcept = default;

    Future<T> GetFuture() const { return Future<T>{ m_FutureState }; }

    template <typename U = T, REQUIRES(std::is_void<U>::value)>
    void SetValue()
    {
        m_FutureState->SetValue();
    }

    template <typename U = T, REQUIRES(!std::is_void<U>::value) >
    void SetValue(U&& val)
    {
        m_FutureState->SetValue(std::forward<U>(val));
    }

    std::stop_token GetStopToken() { return m_FutureState->GetStopToken();  }

private:
    SharedPtr<details::future_shared_state<T>> m_FutureState;
};
} // namespace Onyx::Threading

#undef REQUIRES