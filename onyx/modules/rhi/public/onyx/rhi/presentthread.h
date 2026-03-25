#pragma once

#include <condition_variable>

namespace onyx::rhi {
class GraphicsSystem;
}

namespace onyx::rhi {
class PresentThread : public Thread {
  public:
    PresentThread( GraphicsSystem& graphicsSystem );
    ~PresentThread() override;

    void Shutdown();

    void QueuePresent( uint8_t frameIndex, uint32_t swapchainImageIndex );
    void SetRefreshRate( uint32_t refreshRate );

    void ClearQueue();

  private:
    void onStart() override;
    void onStop() override;
    void onUpdate() override;

  private:
    struct PresentInfo {
        uint8_t BackbufferImageIndex;
        uint8_t FrameIndex;
    };

    std::mutex m_Mutex;
    std::condition_variable m_WaitCondition;

    GraphicsSystem* m_GraphicsSystem;

    Queue< PresentInfo > m_PresentQueue;
    uint32_t m_RefreshRate;
    uint64_t m_TargetFrameTime;

    uint32_t m_PresentCount = 0;
};
} // namespace onyx::rhi
