#pragma once

namespace Onyx
{
    template <typename T, onyxU32 Size>
    LockFreeMPSCBoundedQueue<T, Size>::LockFreeMPSCBoundedQueue()
    {
        for (size_t i = 0; i < Size; ++i)
        {
            m_Buffer[i].m_Sequence = i;
        }
    }

    template <typename T, onyxU32 Size>
    LockFreeMPSCBoundedQueue<T, Size>::LockFreeMPSCBoundedQueue(Self&& rhs) noexcept
    {
        *this = rhs;
    }

    template <typename T, onyxU32 Size>
    LockFreeMPSCBoundedQueue<T, Size>& LockFreeMPSCBoundedQueue<T, Size>::operator=(Self&& rhs) noexcept
    {
        if (this != &rhs)
        {
            m_Buffer = std::move(rhs.m_Buffer);
            m_BufferMask = std::move(rhs.m_BufferMask);
            m_EnqueuePos = rhs.m_EnqueuePos.load();
            m_DequeuePos = rhs.m_DequeuePos;
        }
        return *this;
    }

    template <typename T, onyxU32 Size>
    bool LockFreeMPSCBoundedQueue<T, Size>::Push(T&& data)
    {
        Node* node;
        size_t pos = m_EnqueuePos.load(std::memory_order_relaxed);
        for (;;)
        {
            node = &m_Buffer[pos & m_BufferMask];
            const size_t seq = node->m_Sequence.load(std::memory_order_acquire);
            const ptrdiff_t dif = static_cast<ptrdiff_t>(seq) - static_cast<ptrdiff_t>(pos);
            if (dif == 0)
            {
                if (m_EnqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                {
                    break;
                }
            }
            else if (dif < 0)
            {
                return false;
            }
            else
            {
                pos = m_EnqueuePos.load(std::memory_order_relaxed);
            }
        }

        node->m_Data = std::forward<T>(data);
        node->m_Sequence.store(pos + 1, std::memory_order_release);

        return true;
    }

    template <typename T, onyxU32 Size>
    bool LockFreeMPSCBoundedQueue<T, Size>::Pop(T& data)
    {
        const size_t dequeuePosition = m_DequeuePos;
        Node* node = &m_Buffer[dequeuePosition & m_BufferMask];

        const size_t seq = node->m_Sequence.load(std::memory_order_acquire);
        const ptrdiff_t dif = static_cast<ptrdiff_t>(seq) - static_cast<ptrdiff_t>(dequeuePosition + 1);
        if (dif == 0)
        {
            m_DequeuePos = dequeuePosition + 1;
        }
        else if (dif < 0)
        {
            return false;
        }
        else
        {
            //ASSERT(false , "This is a single consumer queue, another thread accessed the queue!");
        }

        data = std::move(node->m_Data);
        node->m_Sequence.store(dequeuePosition + m_BufferMask + 1, std::memory_order_release);

        return true;
    }
}
