#pragma once

namespace Onyx::Threading
{
    /**
     * @brief The MPMCBoundedQueue class implements bounded
     * multi-producers/multi-consumers lock-free queue.
     * Doesn't accept non-movable types as T.
     * Inspired by Dmitry Vyukov's mpmc queue.
     * http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
     */
    template <typename T>
    class LockFreeMPMCBoundedQueue
    {
        static_assert(std::is_move_constructible_v<T>, "Should be of movable type");

    public:
        explicit LockFreeMPMCBoundedQueue(onyxU32 size);
        LockFreeMPMCBoundedQueue(LockFreeMPMCBoundedQueue&& rhs) noexcept;
        LockFreeMPMCBoundedQueue& operator=(LockFreeMPMCBoundedQueue&& rhs) noexcept;

        bool Push(T&& data);
        bool Pop(T& data);
    private:
        struct Node
        {
            std::atomic<onyxS32> sequence;
            T data;

            Node() = default;

            Node(const Node&) = delete;
            Node& operator=(const Node&) = delete;

            Node(Node&& rhs)
                : sequence(rhs.sequence.load())
                , data(std::move(rhs.data))
            {
            }

            Node& operator=(Node&& rhs)
            {
                sequence = rhs.sequence.load();
                data = std::move(rhs.data);

                return *this;
            }
        };

    private:
        typedef char Cacheline[64];

        Cacheline pad0{};
        std::vector<Node> m_Buffer;
        onyxU32 m_BufferMask = 0;
        Cacheline pad1{};
        Atomic<onyxS32> m_EnqueuePos = 0;
        Cacheline pad2{};
        Atomic<onyxS32> m_DequeuePos = 0;
        Cacheline pad3{};
    };
}

#include <onyx/thread/container/lockfreempmcboundedqueue.hpp>