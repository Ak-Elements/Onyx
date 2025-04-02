#pragma once

namespace Onyx
{
    template <typename ValueT, onyxU32 PoolSize>
    class StaticObjectPool
    {
    public:
        StaticObjectPool()
        {
            m_Objects = static_cast<ValueT*>(new char[sizeof(ValueT) * PoolSize]);

            for (onyxU32 i = 0; i < PoolSize; ++i)
                m_FreeIndices[i] = i;
        }

        ~StaticObjectPool()
        {
            ONYX_ASSERT(static_cast<onyxU32>(m_FreeIndices.size()) == PoolSize, "Objects are still held. Desctruction of the pool will invalidate the memory.");
            delete[] m_Objects;
        }

        onyxU32 AcquireObject()
        {
            ONYX_ASSERT(m_FreeIndices < PoolSize, "No free object available");

            const onyxU32 freeIndex = m_FreeIndices[m_FreeIndicesHead++];
            return freeIndex;
        }

    private:
        ValueT* m_Objects;

        onyxU32 m_FreeIndicesHead = 0;
        std::array<onyxU32, PoolSize> m_FreeIndices;
    };

    //// Simple fixed size object pool for contiguous memory of objects
    //template <typename ObjectType, onyxS32 PoolSize, typename HandleType = onyxS32>
    //class StaticObjectPool
    //{
    //public:
    //    static_assert(std::is_integral_v<HandleType>, "HandleType must be an integral type.");
    //    static_assert(std::numeric_limits<HandleType>::max() >= PoolSize, "HandleType is not covering the full range for the chosen PoolSize.");

    //    StaticObjectPool() = default;
    //    ~StaticObjectPool()
    //    {
    //        ONYX_ASSERT(m_FreeIndicesHead == 0, "Object pool has still objects acquired.");
    //    }

    //    void Init()
    //    {
    //        m_Objects = (ObjectType*)malloc(sizeof(ObjectType) * PoolSize);
    //        /*m_FreeIndices = { } [PoolSize] ;*/

    //        for (HandleType i = 0; i < PoolSize; ++i)
    //        {
    //            m_FreeIndices[i] = i;
    //        }
    //    }

    //    void Clear()
    //    {
    //        //m_Objects.clear();
    //    }

    //    HandleType AcquireObject()
    //    {
    //        ONYX_ASSERT(m_FreeIndicesHead < PoolSize, "No free object available");

    //        const onyxU32 freeIndex = m_FreeIndices[m_FreeIndicesHead++];
    //        return freeIndex;
    //    }

    //    void ReleaseObject(HandleType handle)
    //    {
    //        ONYX_ASSERT(handle < PoolSize, "Handle is outside of the pool");
    //        m_Objects[handle] = nullptr; // TODO: should we only do this in debug?
    //        m_FreeIndices[--m_FreeIndicesHead] = handle;
    //    }

    //    ObjectType* operator[](HandleType handle)
    //    {
    //        ONYX_ASSERT(handle < PoolSize, "Handle is outside of the pool");
    //        return &m_Objects[handle];
    //    }

    //    const ObjectType* operator[](HandleType handle) const
    //    {
    //        ONYX_ASSERT(handle < PoolSize, "Handle is outside of the pool");
    //        return &m_Objects[handle];
    //    }

    //private:
    //    onyxU32 m_FreeIndicesHead = 0;

    //    ObjectType* m_Objects;
    //    std::array<HandleType, PoolSize> m_FreeIndices;
    //};
}