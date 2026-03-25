#pragma once

namespace onyx::threading {
/**
 * @brief The MPMCBoundedQueue class implements bounded
 * multi-producers/multi-consumers lock-free queue.
 * Doesn't accept non-movable types as T.
 * Inspired by Dmitry Vyukov's mpmc queue.
 * http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 */
template < typename T >
class LockFreeMPMCBoundedQueue {
    static_assert( std::is_move_constructible_v< T >, "Should be of movable type" );

  public:
    explicit LockFreeMPMCBoundedQueue( uint32_t size );
    LockFreeMPMCBoundedQueue( LockFreeMPMCBoundedQueue&& rhs ) noexcept;
    LockFreeMPMCBoundedQueue& operator=( LockFreeMPMCBoundedQueue&& rhs ) noexcept;

    bool push( T&& data );
    bool pop( T& data );

  private:
    struct Node {
        std::atomic< int32_t > Sequence;
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

  private:
    typedef char Cacheline[ 64 ];

    Cacheline m_pad0{};
    std::vector< Node > m_buffer;
    uint32_t m_bufferMask = 0;
    Cacheline m_pad1{};
    Atomic< int32_t > m_enqueuePos = 0;
    Cacheline m_pad2{};
    Atomic< int32_t > m_dequeuePos = 0;
    Cacheline m_pad3{};
};
} // namespace onyx::threading

#include <onyx/thread/container/lockfreempmcboundedqueue.hpp>