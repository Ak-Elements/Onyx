#pragma once

#include <onyx/onyx_types.h>

namespace Onyx::Graphics
{
    class MemoryPool
    {
    public:
        MemoryPool(onyxU32 dataSize, onyxU32 capacity)
            : m_ObjectSize(dataSize)
            , m_Capacity(capacity)
        {
            m_Data = new onyxU8[dataSize * capacity];
            m_FreeIndices = new onyxU32[capacity];

            for (onyxU32 i = 0; i < capacity; ++i)
            {
                m_FreeIndices[i] = i;
            }
        }

    protected:
        onyxU32 AcquireIndex()
        {
            ONYX_ASSERT(m_FreeIndicesHead < m_Capacity, "No free object available");
            const onyxU32 freeIndex = m_FreeIndices[m_FreeIndicesHead++];
            return freeIndex;
        }

        void ReleaseIndex(onyxU32 index)
        {
            ONYX_ASSERT(index < m_Capacity, "Index is not in range of the memory pool");
            m_FreeIndices[--m_FreeIndicesHead] = index;
        }

        void* Get(onyxU32 index)
        {
            ONYX_ASSERT(index < m_Capacity, "Index is not in range of the memory pool");
            return &m_Data[index * m_ObjectSize];
        }

        const void* Get(onyxU32 index) const
        {
            ONYX_ASSERT(index < m_Capacity, "Index is not in range of the memory pool");
            return &m_Data[index * m_ObjectSize];
        }

    private:
        onyxU32 m_FreeIndicesHead = 0;
        onyxU8* m_Data = nullptr;
        onyxU32* m_FreeIndices = nullptr;

        onyxU32 m_ObjectSize = 0;
        onyxU32 m_Capacity = 0;
    };

    template <typename T, onyxU32 Capacity>
    class GraphicsResourcePool : public MemoryPool
    {
    public:
        GraphicsResourcePool()
            : MemoryPool(sizeof(T), Capacity)
        {
        }

        template <typename... Args>
        T* AcquireAndEmplace(onyxU32& outIndex, Args&&... args)
        {
            const onyxU32 index = AcquireIndex();
            T* obj = new (Get(index)) T(std::forward<Args>(args)...);
            outIndex = index;
            return obj;
        }

        void Release(const onyxU32 index)
        {
            T* obj = static_cast<T*>(Get(index));
            obj->~T();

            ReleaseIndex(index);
        }

        T* Get(onyxU32 index)
        {
            return static_cast<T*>(MemoryPool::Get(index));
        }

        //    ObjectType* m_Objects;
    };
}