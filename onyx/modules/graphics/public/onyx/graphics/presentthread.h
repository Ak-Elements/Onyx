#pragma once

namespace Onyx::Graphics
{
    class GraphicsApi;
}

namespace Onyx::Graphics
{
    class PresentThread : public Thread
    {
    public:
        PresentThread(GraphicsApi& graphicsApi);
        ~PresentThread() override;

        void Shutdown();

        void QueuePresent(onyxU8 frameIndex, onyxU32 swapchainImageIndex);
        void SetRefreshRate(onyxU32 refreshRate);

        void ClearQueue();

    private:
        void OnStart() override;
        void OnStop() override;
        void OnUpdate() override;

    private:
        struct PresentInfo
        {
            onyxU32 BackbufferImageIndex;
            onyxU8 FrameIndex;
        };

        std::mutex m_Mutex;
        std::condition_variable m_WaitCondition;

        GraphicsApi* m_GraphicsApi;

        Queue<PresentInfo> m_PresentQueue;
        onyxU32 m_RefreshRate;
        onyxU64 m_TargetFrameTime;

        onyxU32 m_PresentCount = 0;
    };
}
