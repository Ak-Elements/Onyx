#pragma once

#define onyxLock( m ) std::lock_guard< std::mutex > lock{ m };

namespace onyx::threading {
struct AtomicLatch {
    AtomicLatch() = default;
    explicit AtomicLatch( int32_t counterVal )
        : m_counter( counterVal ) {}

    AtomicLatch( const AtomicLatch& other )
        : m_counter( other.m_counter.load() ) {}

    AtomicLatch& operator=( const AtomicLatch& other ) {
        if ( this == &other )
            return *this;
        m_counter = other.m_counter.load();
        return *this;
    }

    void setCounter( int32_t counterVal ) { m_counter = counterVal; }
    void increment( int32_t val ) { m_counter += val; }

    bool decrement() {
        if ( m_counter == 0 )
            return true;

        if ( m_counter.fetch_sub( 1 ) == 1 ) {
            m_counter.notify_all();
            return true;
        }

        return false;
    }

    void wait() {
        if ( m_counter == 0 )
            return;

        int32_t counter = m_counter;
        while ( counter != 0 ) {
            std::atomic_wait( &m_counter, counter );
            counter = m_counter;
        }
    }

  private:
    Atomic< int32_t > m_counter = 0;
};
} // namespace onyx::threading
