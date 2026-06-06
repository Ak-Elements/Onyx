#pragma once

#include <onyx/onyx_types.h>

namespace onyx::rhi {
class MemoryPool {
  public:
    MemoryPool( uint32_t dataSize, uint32_t capacity )
        : m_ObjectSize( dataSize )
#if ONYX_ASSERT_ENABLED
        , m_Capacity( capacity )
#endif
    {
        m_Data = new uint8_t[ dataSize * capacity ];
        m_FreeIndices = new uint32_t[ capacity ];

        for ( uint32_t i = 0; i < capacity; ++i ) {
            m_FreeIndices[ i ] = i;
        }
    }

  protected:
    uint32_t AcquireIndex() {
        ONYX_ASSERT( m_FreeIndicesHead < m_Capacity, "No free object available" );
        const uint32_t freeIndex = m_FreeIndices[ m_FreeIndicesHead++ ];
        return freeIndex;
    }

    void ReleaseIndex( uint32_t index ) {
        ONYX_ASSERT( index < m_Capacity, "Index is not in range of the memory pool" );
        m_FreeIndices[ --m_FreeIndicesHead ] = index;
    }

    void* Get( uint32_t index ) {
        ONYX_ASSERT( index < m_Capacity, "Index is not in range of the memory pool" );
        return &m_Data[ index * m_ObjectSize ];
    }

    const void* Get( uint32_t index ) const {
        ONYX_ASSERT( index < m_Capacity, "Index is not in range of the memory pool" );
        return &m_Data[ index * m_ObjectSize ];
    }

  private:
    uint32_t m_FreeIndicesHead = 0;
    uint8_t* m_Data = nullptr;
    uint32_t* m_FreeIndices = nullptr;

    uint32_t m_ObjectSize = 0;
#if ONYX_ASSERT_ENABLED
    uint32_t m_Capacity = 0;
#endif
};

template < typename T, uint32_t Capacity >
class GraphicsResourcePool : public MemoryPool {
  public:
    GraphicsResourcePool()
        : MemoryPool( sizeof( T ), Capacity ) {}

    template < typename... Args >
    T* AcquireAndEmplace( uint32_t& outIndex, Args&&... args ) {
        const uint32_t index = AcquireIndex();
        T* obj = new ( Get( index ) ) T( std::forward< Args >( args )... );
        outIndex = index;
        return obj;
    }

    void Release( const uint32_t index ) {
        T* obj = static_cast< T* >( Get( index ) );
        obj->~T();

        ReleaseIndex( index );
    }

    T* Get( uint32_t index ) { return static_cast< T* >( MemoryPool::Get( index ) ); }
};
} // namespace onyx::rhi