#pragma once

namespace onyx {
template < typename ValueT, uint32_t PoolSize >
class StaticObjectPool {
  public:
    StaticObjectPool() {
        m_objects = static_cast< ValueT* >( new char[ sizeof( ValueT ) * PoolSize ] );

        for ( uint32_t i = 0; i < PoolSize; ++i )
            m_freeIndices[ i ] = i;
    }

    ~StaticObjectPool() {
        ONYX_ASSERT( static_cast< uint32_t >( m_FreeIndices.size() ) == PoolSize,
                     "Objects are still held. Desctruction of the pool will invalidate the memory." );
        delete[] m_objects;
    }

    uint32_t acquireObject() {
        ONYX_ASSERT( m_FreeIndices < PoolSize, "No free object available" );

        const uint32_t freeIndex = m_freeIndices[ m_freeIndicesHead++ ];
        return freeIndex;
    }

  private:
    ValueT* m_objects;

    uint32_t m_freeIndicesHead = 0;
    std::array< uint32_t, PoolSize > m_freeIndices;
};

} // namespace onyx