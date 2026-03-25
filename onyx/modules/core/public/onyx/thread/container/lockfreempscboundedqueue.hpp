#pragma once

namespace onyx {
template < typename T, uint32_t Size >
LockFreeMPSCBoundedQueue< T, Size >::LockFreeMPSCBoundedQueue() {
    for ( size_t i = 0; i < Size; ++i ) {
        m_buffer[ i ].Sequence = i;
    }
}

template < typename T, uint32_t Size >
LockFreeMPSCBoundedQueue< T, Size >::LockFreeMPSCBoundedQueue( Self&& rhs ) noexcept {
    *this = rhs;
}

template < typename T, uint32_t Size >
LockFreeMPSCBoundedQueue< T, Size >& LockFreeMPSCBoundedQueue< T, Size >::operator=( Self&& rhs ) noexcept {
    if ( this != &rhs ) {
        m_buffer = std::move( rhs.m_buffer );
        m_bufferMask = rhs.m_bufferMask;
        m_enqueuePos = rhs.m_enqueuePos.load();
        m_dequeuePos = rhs.m_dequeuePos;
    }
    return *this;
}

template < typename T, uint32_t Size >
bool LockFreeMPSCBoundedQueue< T, Size >::push( T&& data ) {
    Node* node;
    size_t pos = m_enqueuePos.load( std::memory_order::relaxed );
    for ( ;; ) {
        node = &m_buffer[ pos & m_bufferMask ];
        const size_t seq = node->Sequence.load( std::memory_order::acquire );
        const ptrdiff_t dif = static_cast< ptrdiff_t >( seq ) - static_cast< ptrdiff_t >( pos );
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

    node->Data = std::forward< T >( data );
    node->Sequence.store( pos + 1, std::memory_order::release );

    return true;
}

template < typename T, uint32_t Size >
bool LockFreeMPSCBoundedQueue< T, Size >::pop( T& data ) {
    const size_t dequeuePosition = m_dequeuePos;
    Node* node = &m_buffer[ dequeuePosition & m_bufferMask ];

    const size_t seq = node->Sequence.load( std::memory_order::acquire );
    const ptrdiff_t dif = static_cast< ptrdiff_t >( seq ) - static_cast< ptrdiff_t >( dequeuePosition + 1 );
    if ( dif == 0 ) {
        m_dequeuePos = dequeuePosition + 1;
    } else if ( dif < 0 ) {
        return false;
    } else {
        // ASSERT(false , "This is a single consumer queue, another thread accessed the queue!");
    }

    data = std::move( node->Data );
    node->Sequence.store( dequeuePosition + m_bufferMask + 1, std::memory_order::release );

    return true;
}
} // namespace onyx
