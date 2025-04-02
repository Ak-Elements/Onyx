#include <onyx/thread/thread.h>

namespace Onyx
{
    std::thread::id Thread::MAIN_THREAD_ID;

    Thread::~Thread()
    {
        if (m_Stopped == false)
        {
            Stop(true);
        }
        else if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    void Thread::Start()
    {
        m_Thread = std::thread([this]()
        {
            Run();
        });
    }

    void Thread::Stop(bool waitForStop)
    {
        if (m_Stopped)
            return;

        m_Stopped = true;

        if (waitForStop)
        {
            m_Thread.join();
        }
    }

    void Thread::Sleep(onyxU64 milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void Thread::Run()
    {
        OnStart();
        OnUpdate();
        OnStop();
    }

}
