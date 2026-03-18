#pragma once
#include <onyx/assets/assetloadrequest.h>
#include <onyx/assets/assetserializer.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <onyx/assets/assethandle.h>

namespace onyx::assets
{
    struct AssetMetaData;
    class AssetInterface;

    class AssetIOHandler
    {
    public:
        void RequestLoad(const AssetMetaData& metaData, const AssetHandle<AssetInterface>& assetHandle, const UniquePtr<IAssetSerializer>& serializer, IEngine* engine)
        {
            if (m_LoadRequests.contains(metaData.Id))
                return;

            UniquePtr<AssetLoadRequest> loadRequest = MakeUnique<AssetLoadRequest>();
            loadRequest->Engine = engine;
            loadRequest->MetaData = metaData;
            loadRequest->Asset = assetHandle;
            loadRequest->Serializer = serializer.get();
            loadRequest->OnLoadFinished.Connect<&AssetIOHandler::OnAssetLoadFinished>(this);
            loadRequest->Start(m_LoaderThreadPool);

            m_LoadRequests[metaData.Id] = std::move(loadRequest);
        }

#if ONYX_IS_EDITOR
        void RequestSave(const AssetMetaData& metaData, const AssetHandle<AssetInterface>& assetHandle, const UniquePtr<IAssetSerializer>& serializer, const IEngine* engine)
        {
            if (m_SaveRequests.contains(metaData.Id))
                return;

            UniquePtr<AssetSaveRequest> saveRequest = MakeUnique<AssetSaveRequest>();
            saveRequest->Engine = engine;
            saveRequest->MetaData = metaData;
            saveRequest->Asset = assetHandle;
            saveRequest->Serializer = serializer.get();
            saveRequest->OnSaveFinished.Connect<&AssetIOHandler::OnAssetSaveFinished>(this);
            saveRequest->Start(m_LoaderThreadPool);

            m_SaveRequests[metaData.Id] = std::move(saveRequest);
        }
#endif

    private:
        void OnAssetLoadFinished(AssetHandle<AssetInterface>& handle);
#if ONYX_IS_EDITOR
        void OnAssetSaveFinished(const AssetHandle<AssetInterface>& handle);
#endif
    private:
        // default initalize to the amount of logical cores available
        HashMap<AssetId, UniquePtr<AssetLoadRequest>> m_LoadRequests;
#if ONYX_IS_EDITOR
        HashMap<AssetId, UniquePtr<AssetSaveRequest>> m_SaveRequests;
#endif

#if ONYX_PROFILER_ENABLED
        threading::ThreadPool m_LoaderThreadPool { threading::ThreadPoolOptions(), "Asset Loader" };
#else
        threading::ThreadPool m_LoaderThreadPool { threading::ThreadPoolOptions(1) };
#endif
    };
}
