#pragma once

namespace onyx {
template < typename T, uint32_t Size >
class LockFreeMPSCBoundedQueue {
    static_assert( std::is_move_constructible_v< T >, "Should be of movable type" );
    static_assert( ( Size >= 2 ) && ( ( Size & ( Size - 1 ) ) == 0 ), "Buffer size has to be a power of 2" );

    using Self = LockFreeMPSCBoundedQueue< T, Size >;

  public:
    LockFreeMPSCBoundedQueue();
    LockFreeMPSCBoundedQueue( Self&& rhs ) noexcept;
    LockFreeMPSCBoundedQueue& operator=( Self&& rhs ) noexcept;

    bool push( T&& data );
    bool pop( T& data );

  private:
    struct Node {
        std::atomic< size_t > Sequence;
        T Data;

        Node() = default;

        Node( const Node& ) = delete;
        Node& operator=( const Node& ) = delete;

        Node( Node&& rhs ) noexcept
            : Sequence( rhs.Sequence.load() )
            , Data( std::move( rhs.Data ) ) {}

        Node& operator=( Node&& rhs ) noexcept {
            Sequence = rhs.Sequence.load();
            Data = std::move( rhs.Data );

            return *this;
        }
    };

    using CachelinePadding = char[ 64 ];

    CachelinePadding m_pad0 = {};
    Node m_buffer[ Size ];
    size_t m_bufferMask = Size - 1;
    CachelinePadding m_pad1 = {};
    Atomic< size_t > m_enqueuePos = 0;
    CachelinePadding m_pad2 = {};
    size_t m_dequeuePos = 0;
    CachelinePadding m_pad3 = {};
};
} // namespace onyx

#include <onyx/thread/container/lockfreempscboundedqueue.hpp>
