#pragma once

namespace Onyx
{
    template <typename T, onyxU32 Size>
    class LockFreeMPSCBoundedQueue
    {
        static_assert(std::is_move_constructible_v<T>, "Should be of movable type");
        static_assert((Size >= 2) && ((Size& (Size - 1)) == 0), "Buffer size has to be a power of 2");

        using Self = LockFreeMPSCBoundedQueue<T, Size>;

    public:
        LockFreeMPSCBoundedQueue();
        LockFreeMPSCBoundedQueue(Self&& rhs) noexcept;
        LockFreeMPSCBoundedQueue& operator=(Self&& rhs) noexcept;

        bool Push(T&& data);
        bool Pop(T& data);

    private:
        struct Node
        {
            std::atomic<size_t> m_Sequence;
            T m_Data;

            Node() = default;

            Node(const Node&) = delete;
            Node& operator=(const Node&) = delete;

            Node(Node&& rhs) noexcept
                : m_Sequence(rhs.m_Sequence.load())
                , m_Data(std::move(rhs.m_Data))
            {
            }

            Node& operator=(Node&& rhs) noexcept
            {
                m_Sequence = rhs.m_Sequence.load();
                m_Data = std::move(rhs.m_Data);

                return *this;
            }
        };
        
        using CachelinePadding = char[64];

        CachelinePadding m_Pad0 = {};
        Node m_Buffer[Size];
        const size_t m_BufferMask = Size - 1;
        CachelinePadding m_Pad1 = {};
        Atomic<size_t> m_EnqueuePos = 0;
        CachelinePadding m_Pad2 = {};
        size_t m_DequeuePos = 0;
        CachelinePadding m_Pad3 = {};
    };
}

#include <onyx/thread/container/lockfreempscboundedqueue.hpp>
