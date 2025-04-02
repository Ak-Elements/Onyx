#pragma once

#define onyxLock(m) std::lock_guard<std::mutex> lock { m };

namespace Onyx
{
    namespace Threading
    {
        struct AtomicLatch
        {
            AtomicLatch() = default;
            AtomicLatch(onyxS32 counterVal) : m_Counter(counterVal) {}

            AtomicLatch(const AtomicLatch& other)
                : m_Counter(other.m_Counter.load()) {}

            AtomicLatch& operator=(const AtomicLatch& other)
            {
                if (this == &other)
                    return *this;
                m_Counter = other.m_Counter.load();
                return *this;
            }

            void SetCounter(onyxS32 counterVal) { m_Counter = counterVal; }
            void Increment(onyxS32 val)
            {
                m_Counter += val;
            }

            bool Decrement()
            {
                if (m_Counter == 0)
                    return true;

                if (m_Counter.fetch_sub(1) == 1)
                {
                    m_Counter.notify_all();
                    return true;
                }

                return false;
            }

            void Wait()
            {
                if (m_Counter == 0)
                    return;

                onyxS32 counter = m_Counter;
                while (counter != 0)
                {
                    std::atomic_wait(&m_Counter, counter);
                    counter = m_Counter;
                }

            }

        private:
            Atomic<onyxS32> m_Counter = 0;
        };
    }
}
