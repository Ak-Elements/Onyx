#pragma once

#include <thread>

namespace Onyx
{
    class Thread
    {
    public:
        static std::thread::id MAIN_THREAD_ID;

        Thread() = default;
        virtual ~Thread();

        Thread(const Thread& other) = delete;
        Thread& operator=(const Thread& other) = delete;

        Thread(Thread&& other) = delete;
        Thread& operator=(Thread&& other) = delete;

        void Start();
        void Stop(bool waitForStop);
        static void Sleep(onyxU64 milliseconds);
    protected:
        bool IsRunning() const { return m_Stopped == false; }
    private:
        void Run();
        
        virtual void OnStart() {}
        virtual void OnStop() {}
        /*
         * should be implemented as
         * while (IsRunning())
         * {
         *      // do work
         * }
         * */
        virtual void OnUpdate() {}

    private:
        std::thread m_Thread;
        Atomic<bool> m_Stopped;
    };
}
