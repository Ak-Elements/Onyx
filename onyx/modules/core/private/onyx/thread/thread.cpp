#include <onyx/thread/thread.h>

namespace onyx {
std::thread::id Thread::s_mainThreadId;

Thread::~Thread() {
    if ( m_stopped == false ) {
        stop( true );
    } else if ( m_thread.joinable() ) {
        m_thread.join();
    }
}

void Thread::start() {
    m_thread = std::thread( [ this ]() { run(); } );
}

void Thread::stop( bool waitForStop ) {
    if ( m_stopped )
        return;

    m_stopped = true;

    if ( waitForStop ) {
        m_thread.join();
    }
}

void Thread::sleep( uint64_t milliseconds ) {
    std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
}

void Thread::run() {
    onStart();
    onUpdate();
    onStop();
}

} // namespace onyx
