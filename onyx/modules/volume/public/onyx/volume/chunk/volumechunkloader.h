#pragma once

#include <onyx/volume/chunk/volumechunkloadrequest.h>
#include <vector>

namespace Onyx::Volume
{
    class VolumeChunkLoader
    {
    public:
        void RequestLoad(const VolumeChunckLoadRequestData& reqData, InplaceFunction<void(const VolumeChunckLoadRequestData&), 64>&& finishedCallback);

    private:
        void Cancel();
        void ClearCanceledTasks();
    private:
        std::unique_ptr<VolumeChunkLoadRequest> m_ActiveLoadRequest;
        std::vector<std::unique_ptr<VolumeChunkLoadRequest>> m_PendingCanceledTasks;
    };
}