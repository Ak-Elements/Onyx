#pragma once

#include <onyx/onyx_types.h>

namespace onyx::rhi {
class MemoryPool {
  public:
    MemoryPool( uint32_t dataSize, uint32_t capacity )
        : m_objectSize( dataSize )
#if ONYX_ASSERT_ENABLED
        , m_capacity( capacity )
#endif
    {
        m_data = new uint8_t[ dataSize * capacity ];
        m_freeIndices = new uint32_t[ capacity ];

        for( uint32_t i = 0; i < capacity; ++i ) {
            m_freeIndices[ i ] = i;
        }
    }

  protected:
    uint32_t acquireIndex() {
        ONYX_ASSERT( m_freeIndicesHead < m_capacity, "No free object available" );
        const uint32_t freeIndex = m_freeIndices[ m_freeIndicesHead++ ];
        return freeIndex;
    }

    void releaseIndex( uint32_t index ) {
        ONYX_ASSERT( index < m_capacity, "Index is not in range of the memory pool" );
        m_freeIndices[ --m_freeIndicesHead ] = index;
    }

    void* get( uint32_t index ) {
        ONYX_ASSERT( index < m_capacity, "Index is not in range of the memory pool" );
        return &m_data[ index * m_objectSize ];
    }

    [[nodiscard]] const void* get( uint32_t index ) const {
        ONYX_ASSERT( index < m_capacity, "Index is not in range of the memory pool" );
        return &m_data[ index * m_objectSize ];
    }

  private:
    uint32_t m_freeIndicesHead = 0;
    uint8_t* m_data = nullptr;
    uint32_t* m_freeIndices = nullptr;

    uint32_t m_objectSize = 0;
#if ONYX_ASSERT_ENABLED
    uint32_t m_capacity = 0;
#endif
};

template < typename T, uint32_t Capacity >
class GraphicsResourcePool : public MemoryPool {
  public:
    GraphicsResourcePool()
        : MemoryPool( sizeof( T ), Capacity ) {}

    template < typename... Args >
    T* acquireAndEmplace( uint32_t& outIndex, Args&&... args ) {
        const uint32_t index = acquireIndex();
        T* obj = new( get( index ) ) T( std::forward< Args >( args )... );
        outIndex = index;
        return obj;
    }

    void release( const uint32_t index ) {
        T* obj = static_cast< T* >( get( index ) );
        obj->~T();

        releaseIndex( index );
    }

    T* get( uint32_t index ) { return static_cast< T* >( MemoryPool::get( index ) ); }
};
} // namespace onyx::rhi
