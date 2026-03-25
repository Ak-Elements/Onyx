#pragma once

#include <onyx/thread/container/lockfreempmcboundedqueue.h>

namespace onyx::threading {
/// Implementation

template < typename T >
inline LockFreeMPMCBoundedQueue< T >::LockFreeMPMCBoundedQueue( uint32_t size )
    : m_buffer( size )
    , m_bufferMask( size - 1 ) {
    const bool sizeIsPowerOf2 = ( size >= 2 ) && ( ( size & ( size - 1 ) ) == 0 );
    if ( !sizeIsPowerOf2 ) {
        throw std::invalid_argument( "buffer size should be a power of 2" );
    }

    for ( uint32_t i = 0; i < size; ++i ) {
        m_buffer[ i ].Sequence = i;
    }
}

template < typename T >
inline LockFreeMPMCBoundedQueue< T >::LockFreeMPMCBoundedQueue( LockFreeMPMCBoundedQueue&& rhs ) noexcept {
    *this = rhs;
}

template < typename T >
inline LockFreeMPMCBoundedQueue< T >& LockFreeMPMCBoundedQueue< T >::operator=(
    LockFreeMPMCBoundedQueue&& rhs ) noexcept {
    if ( this != &rhs ) {
        m_buffer = std::move( rhs.m_buffer );
        m_bufferMask = rhs.m_bufferMask;
        m_enqueuePos = rhs.m_enqueuePos.load();
        m_dequeuePos = rhs.m_dequeuePos.load();
    }
    return *this;
}

template < typename T >
inline bool LockFreeMPMCBoundedQueue< T >::push( T&& data ) {
    Node* node;
    int32_t pos = m_enqueuePos.load( std::memory_order::relaxed );
    for ( ;; ) {
        node = &m_buffer[ pos & m_bufferMask ];
        int32_t seq = node->Sequence.load( std::memory_order::acquire );
        std::ptrdiff_t dif = static_cast< std::ptrdiff_t >( seq ) - static_cast< std::ptrdiff_t >( pos );
        if ( dif == 0 ) {
            if ( m_enqueuePos.compare_exchange_weak( pos, pos + 1, std::memory_order::relaxed ) ) {
                break;
            }
        } else if ( dif < 0 ) {
            return false;
        } else {
            pos = m_enqueuePos.load( std::memory_order::relaxed );
        }
    }

    node->Data = std::move( data );
    node->Sequence.store( pos + 1, std::memory_order::release );
    return true;
}

template < typename T >
inline bool LockFreeMPMCBoundedQueue< T >::pop( T& data ) {
    Node* node;
    int32_t pos = m_dequeuePos.load( std::memory_order::relaxed );
    for ( ;; ) {
        node = &m_buffer[ pos & m_bufferMask ];
        int32_t seq = node->Sequence.load( std::memory_order::acquire );
        std::ptrdiff_t dif = static_cast< std::ptrdiff_t >( seq ) - static_cast< std::ptrdiff_t >( pos + 1 );
        if ( dif == 0 ) {
            if ( m_dequeuePos.compare_exchange_weak( pos, pos + 1, std::memory_order::relaxed ) ) {
                break;
            }
        } else if ( dif < 0 ) {
            return false;
        } else {
            pos = m_dequeuePos.load( std::memory_order::relaxed );
        }
    }

    data = std::move( node->Data );

    node->Sequence.store( pos + m_bufferMask + 1, std::memory_order::release );

    return true;
}

} // namespace onyx::threading
