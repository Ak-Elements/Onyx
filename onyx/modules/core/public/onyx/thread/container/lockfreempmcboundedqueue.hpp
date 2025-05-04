#pragma once

#include <onyx/thread/container/lockfreempmcboundedqueue.h>

namespace Onyx
{

namespace Threading
{
/// Implementation

template <typename T>
inline LockFreeMPMCBoundedQueue<T>::LockFreeMPMCBoundedQueue(onyxU32 size)
    : m_Buffer(size)
    , m_BufferMask(size - 1)
{
    const bool size_is_power_of_2 = (size >= 2) && ((size & (size - 1)) == 0);
    if(!size_is_power_of_2)
    {
        throw std::invalid_argument("buffer size should be a power of 2");
    }

    for(onyxU32 i = 0; i < size; ++i)
    {
        m_Buffer[i].sequence = i;
    }
}

template <typename T>
inline LockFreeMPMCBoundedQueue<T>::LockFreeMPMCBoundedQueue(LockFreeMPMCBoundedQueue&& rhs) noexcept
{
    *this = rhs;
}

template <typename T>
inline LockFreeMPMCBoundedQueue<T>& LockFreeMPMCBoundedQueue<T>::operator=(LockFreeMPMCBoundedQueue&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_Buffer = std::move(rhs.m_Buffer);
        m_BufferMask = std::move(rhs.m_BufferMask);
        m_EnqueuePos = rhs.m_EnqueuePos.load();
        m_DequeuePos = rhs.m_DequeuePos.load();
    }
    return *this;
}

template <typename T>
inline bool LockFreeMPMCBoundedQueue<T>::Push(T&& data)
{
    Node* node;
    onyxS32 pos = m_EnqueuePos.load(std::memory_order_relaxed);
    for(;;)
    {
        node = &m_Buffer[pos & m_BufferMask];
        onyxS32 seq = node->sequence.load(std::memory_order_acquire);
        onyxPtrDiff dif = static_cast<onyxPtrDiff>(seq) - static_cast<onyxPtrDiff>(pos);
        if(dif == 0)
        {
            if(m_EnqueuePos.compare_exchange_weak(
                   pos, pos + 1, std::memory_order_relaxed))
            {
                break;
            }
        }
        else if(dif < 0)
        {
            return false;
        }
        else
        {
            pos = m_EnqueuePos.load(std::memory_order_relaxed);
        }
    }

    node->data = std::move(data);
    node->sequence.store(pos + 1, std::memory_order_release);
    return true;
}

template <typename T>
inline bool LockFreeMPMCBoundedQueue<T>::Pop(T& data)
{
    Node* node;
    onyxS32 pos = m_DequeuePos.load(std::memory_order_relaxed);
    for(;;)
    {
        node = &m_Buffer[pos & m_BufferMask];
        onyxS32 seq = node->sequence.load(std::memory_order_acquire);
        onyxPtrDiff dif = static_cast<onyxPtrDiff>(seq) - static_cast<onyxPtrDiff>(pos + 1);
        if(dif == 0)
        {
            if(m_DequeuePos.compare_exchange_weak(
                   pos, pos + 1, std::memory_order_relaxed))
            {
                break;
            }
        }
        else if(dif < 0)
        {
            return false;
        }
        else
        {
            pos = m_DequeuePos.load(std::memory_order_relaxed);
        }
    }

    data = std::move(node->data);

    node->sequence.store(
        pos + m_BufferMask + 1, std::memory_order_release);

    return true;
}

}

}
