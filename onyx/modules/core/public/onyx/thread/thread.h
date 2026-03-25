#pragma once

#include <thread>

namespace onyx {
class Thread {
  public:
    static std::thread::id s_mainThreadId;

    Thread() = default;
    virtual ~Thread();

    Thread( const Thread& other ) = delete;
    Thread& operator=( const Thread& other ) = delete;

    Thread( Thread&& other ) = delete;
    Thread& operator=( Thread&& other ) = delete;

    void start();
    void stop( bool waitForStop );
    static void sleep( uint64_t milliseconds );

  protected:
    bool isRunning() const { return m_stopped == false; }

  private:
    void run();

    virtual void onStart() {}
    virtual void onStop() {}
    /*
     * should be implemented as
     * while (IsRunning())
     * {
     *      // do work
     * }
     * */
    virtual void onUpdate() {}

  private:
    std::thread m_thread;
    Atomic< bool > m_stopped;
};
} // namespace onyx
