#include <onyx/rhi/presentthread.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/swapchain.h>
#include <onyx/profiler/profiler.h>

namespace Onyx::Graphics
{
    PresentThread::PresentThread(GraphicsSystem& graphicsSystem)
        : m_GraphicsSystem(&graphicsSystem)
    {
    }

    PresentThread::~PresentThread() = default;

    void PresentThread::Shutdown()
    {
        Stop(false);
        m_WaitCondition.notify_one();

        std::unique_lock lock(m_Mutex);
        m_WaitCondition.wait(lock);
    }

    void PresentThread::QueuePresent(onyxU8 frameIndex, onyxU32 swapchainImageIndex)
    {
        {
            std::lock_guard lock(m_Mutex);
            m_PresentQueue.emplace(swapchainImageIndex, frameIndex);
        }

        m_WaitCondition.notify_one();
    }

    void PresentThread::SetRefreshRate(onyxU32 refreshRate)
    {
        m_RefreshRate = refreshRate;
        m_TargetFrameTime = 1000000000 / std::max<onyxU32>(m_RefreshRate, 1);
    }

    void PresentThread::ClearQueue()
    {
        std::lock_guard lock(m_Mutex);
        Queue<PresentInfo> empty;
        std::swap(m_PresentQueue, empty);
    }

    void PresentThread::OnStart()
    {
        ONYX_PROFILE_SET_THREAD(Present);
    }

    void PresentThread::OnStop()
    {
    }

    void PresentThread::OnUpdate()
    {
        Vulkan::VulkanGraphicsApi& vulkan = m_GraphicsSystem->GetApi<Vulkan::VulkanGraphicsApi>();
        SetRefreshRate(m_GraphicsSystem->GetRefreshRate());

        while (IsRunning())
        {
            ONYX_PROFILE_SECTION(PresentLoop);
            onyxU64 presentStart = Time::GetCurrentNanoseconds();

            std::unique_lock lock(m_Mutex);
            m_WaitCondition.wait(lock, [&]() { return (m_PresentQueue.empty() == false) || (IsRunning() == false); });

            if (IsRunning() == false)
            {
                lock.unlock();
                break;
            }

            PresentInfo presentInfo = m_PresentQueue.front();
            m_PresentQueue.pop();
            lock.unlock();

            {
                ONYX_PROFILE_SECTION(Present);
                Vulkan::SwapChain& swapChain = vulkan.GetSwapChain();
                bool hasPresented = swapChain.Present(presentInfo.FrameIndex, presentInfo.BackbufferImageIndex);
                if (hasPresented == false)
                {
                    ClearQueue();
                    //m_GraphicsSystem->OnWindowResize(swapChain.GetExtent()[0], swapChain.GetExtent()[1]);
                }
            }

            ++m_PresentCount;

            onyxU64 presentDuration = Time::GetCurrentNanoseconds() - presentStart;
            if (presentDuration < m_TargetFrameTime)
            {
                //onyxU64 sleepDuration = m_TargetFrameTime - presentDuration;
                //std::this_thread::sleep_for(std::chrono::nanoseconds(sleepDuration));
            }
        }

        m_WaitCondition.notify_all();
    }
}
 