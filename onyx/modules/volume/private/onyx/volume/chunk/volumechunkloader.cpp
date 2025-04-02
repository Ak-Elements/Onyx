#include <onyx/volume/chunk/volumechunkloader.h>

namespace Onyx::Volume
{
    void VolumeChunkLoader::RequestLoad(const VolumeChunckLoadRequestData& reqData, InplaceFunction<void(const VolumeChunckLoadRequestData&), 64>&& finishedCallback)
    {
        Cancel();
        ClearCanceledTasks();
       
        m_ActiveLoadRequest = MakeUnique<VolumeChunkLoadRequest>(reqData, std::forward<InplaceFunction<void(const VolumeChunckLoadRequestData&), 64>>(finishedCallback));
        m_ActiveLoadRequest->Load();
    }

    void VolumeChunkLoader::Cancel()
    {
        if (m_ActiveLoadRequest)
        {
            m_ActiveLoadRequest->Cancel(false);
            m_PendingCanceledTasks.push_back(std::move(m_ActiveLoadRequest));

            m_ActiveLoadRequest = nullptr;
        }
    }

    void VolumeChunkLoader::ClearCanceledTasks()
    {
        std::erase_if(m_PendingCanceledTasks, [](auto& pendingCanceledTask) { return pendingCanceledTask->IsFinished(); });
    }
}