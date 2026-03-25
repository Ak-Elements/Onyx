#include <onyx/rhi/presentthread.h>

#include <onyx/profiler/profiler.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/swapchain.h>

namespace onyx::rhi {
PresentThread::PresentThread( GraphicsSystem& graphicsSystem )
    : m_GraphicsSystem( &graphicsSystem ) {}

PresentThread::~PresentThread() = default;

void PresentThread::Shutdown() {
    stop( false );
    m_WaitCondition.notify_one();

    std::unique_lock lock( m_Mutex );
    m_WaitCondition.wait( lock );
}

void PresentThread::QueuePresent( uint8_t frameIndex, uint32_t swapchainImageIndex ) {
    {
        std::lock_guard lock( m_Mutex );
        m_PresentQueue.emplace( swapchainImageIndex, frameIndex );
    }

    m_WaitCondition.notify_one();
}

void PresentThread::SetRefreshRate( uint32_t refreshRate ) {
    m_RefreshRate = refreshRate;
    m_TargetFrameTime = 1000000000 / std::max< uint32_t >( m_RefreshRate, 1 );
}

void PresentThread::ClearQueue() {
    std::lock_guard lock( m_Mutex );
    Queue< PresentInfo > empty;
    std::swap( m_PresentQueue, empty );
}

void PresentThread::onStart() {
    ONYX_PROFILE_SET_THREAD( Present );
}

void PresentThread::onStop() {}

void PresentThread::onUpdate() {
    vulkan::VulkanGraphicsApi& vulkan = m_GraphicsSystem->GetApi< vulkan::VulkanGraphicsApi >();
    SetRefreshRate( m_GraphicsSystem->GetRefreshRate() );

    while ( isRunning() ) {
        ONYX_PROFILE_SECTION( PresentLoop );
        uint64_t presentStart = Time::GetCurrentNanoseconds();

        std::unique_lock lock( m_Mutex );
        m_WaitCondition.wait( lock,
                              [ & ]() { return ( m_PresentQueue.empty() == false ) || ( isRunning() == false ); } );

        if ( isRunning() == false ) {
            lock.unlock();
            break;
        }

        PresentInfo presentInfo = m_PresentQueue.front();
        m_PresentQueue.pop();
        lock.unlock();

        {
            ONYX_PROFILE_SECTION( Present );
            vulkan::SwapChain& swapChain = vulkan.GetSwapChain();
            bool hasPresented = swapChain.Present( presentInfo.BackbufferImageIndex );
            if ( hasPresented == false ) {
                ClearQueue();
            }
        }

        ++m_PresentCount;

        uint64_t presentDuration = Time::GetCurrentNanoseconds() - presentStart;
        if ( presentDuration < m_TargetFrameTime ) {
            // uint64_t sleepDuration = m_TargetFrameTime - presentDuration;
            // std::this_thread::sleep_for(std::chrono::nanoseconds(sleepDuration));
        }
    }

    m_WaitCondition.notify_all();
}
} // namespace onyx::rhi
