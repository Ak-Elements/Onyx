#pragma once

#include <onyx/assets/asset.h>

#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <onyx/assets/assethandle.h>

namespace Onyx
{
    class IEngine;
}

namespace Onyx::Assets
{
    struct IAssetSerializer;
    class AssetLoader;
    struct AssetId;
    class AssetInterface;

    struct AssetLoadRequest
    {
    public:
        void Start(Threading::ThreadPool& loaderPool);
        void Cancel();

        IEngine* Engine = nullptr;
        AssetMetaData MetaData;
        AssetHandle<AssetInterface> Asset;
        const IAssetSerializer* Serializer = nullptr;

        Callback<void(AssetHandle<AssetInterface>&)> OnLoadFinished;
    private:
        void Load();

    private:
        Threading::Future<void> m_Future;
    };

#if ONYX_IS_EDITOR
    struct AssetSaveRequest
    {
    public:
        void Start(Threading::ThreadPool& loaderPool);
        void Cancel();

        const IEngine* Engine = nullptr;
        AssetMetaData MetaData;
        AssetHandle<AssetInterface> Asset;
        const IAssetSerializer* Serializer = nullptr;

        Callback<void(const AssetHandle<AssetInterface>&)> OnSaveFinished;
    private:
        void Save();

    private:
        Threading::Future<void> m_Future;
    };
#endif

}
