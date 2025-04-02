#pragma once
#include <onyx/assets/assetloadrequest.h>
#include <onyx/assets/assetserializer.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/threadpool/threadpool.h>

namespace Onyx::Assets
{
    struct AssetMetaData;
    class AssetInterface;

    class AssetIOHandler
    {
    public:
        void RequestLoad(const AssetMetaData& metaData, const Reference<AssetInterface>& assetHandle, const UniquePtr<AssetSerializer>& serializer)
        {
            if (m_LoadRequests.contains(metaData.Id))
                return;

            AssetLoadRequest& loadRequest = m_LoadRequests[metaData.Id];
            loadRequest.Path = metaData.Path;
            loadRequest.Handle = assetHandle;
            loadRequest.Serializer = serializer.get();
            loadRequest.OnLoadFinished.Connect<&AssetIOHandler::OnAssetLoadFinished>(this);
            loadRequest.Start(m_LoaderThreadPool);
        }

#if ONYX_IS_EDITOR
        void RequestSave(const AssetMetaData& metaData, const Reference<AssetInterface>& assetHandle, const UniquePtr<AssetSerializer>& serializer)
        {
            if (m_SaveRequests.contains(metaData.Id))
                return;

            AssetSaveRequest& saveRequest = m_SaveRequests[metaData.Id];
            saveRequest.Path = metaData.Path;
            saveRequest.Handle = assetHandle;
            saveRequest.Serializer = serializer.get();
            saveRequest.OnSaveFinished.Connect<&AssetIOHandler::OnAssetSaveFinished>(this);
            saveRequest.Start(m_LoaderThreadPool);
        }
#endif

    private:
        void OnAssetLoadFinished(Reference<AssetInterface>& handle);
#if ONYX_IS_EDITOR
        void OnAssetSaveFinished(const Reference<AssetInterface>& handle);
#endif
    private:
        // default initalize to the amount of logical cores available
        HashMap<AssetId, AssetLoadRequest> m_LoadRequests;
#if ONYX_IS_EDITOR
        HashMap<AssetId, AssetSaveRequest> m_SaveRequests;
#endif

#if ONYX_PROFILER_ENABLED
        Threading::ThreadPool m_LoaderThreadPool { "Asset Loader" };
#else
        Threading::ThreadPool m_LoaderThreadPool {};
#endif
    };
}
