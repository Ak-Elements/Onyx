#pragma once

namespace Onyx::Volume
{
    class VolumeChunkLoadRequest;
    struct VolumeChunckLoadRequestData;

    class VolumeChunkLoader
    {
    public:
        void RequestLoad(const VolumeChunckLoadRequestData& reqData, InplaceFunction<void(const VolumeChunckLoadRequestData&), 64>&& finishedCallback);

    private:
        void Cancel();
        void ClearCanceledTasks();
    private:
        UniquePtr<VolumeChunkLoadRequest> m_ActiveLoadRequest;
        DynamicArray<UniquePtr<VolumeChunkLoadRequest>> m_PendingCanceledTasks;
    };
}