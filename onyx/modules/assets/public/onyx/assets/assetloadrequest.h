#pragma once

#include <onyx/assets/asset.h>

#include <onyx/thread/async/future.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <onyx/assets/assethandle.h>

namespace onyx {
class IEngine;
}

namespace onyx::assets {
struct IAssetSerializer;
class AssetLoader;
struct AssetId;
class AssetInterface;

struct AssetLoadRequest {
  public:
    void start( threading::ThreadPool& loaderPool );
    void cancel();

    IEngine* Engine = nullptr;
    AssetMetaData MetaData;
    AssetHandle< AssetInterface > Asset;
    const IAssetSerializer* Serializer = nullptr;

    Callback< void( AssetHandle< AssetInterface >& ) > OnLoadFinished;

  private:
    void load();

  private:
    threading::Future< void > m_future;
};

#if ONYX_IS_EDITOR
struct AssetSaveRequest {
  public:
    void start( threading::ThreadPool& loaderPool );
    void cancel();

    const IEngine* Engine = nullptr;
    AssetMetaData MetaData;
    AssetHandle< AssetInterface > Asset;
    const IAssetSerializer* Serializer = nullptr;

    Callback< void( const AssetHandle< AssetInterface >& ) > OnSaveFinished;

  private:
    void save();

  private:
    threading::Future< void > m_future;
};
#endif

} // namespace onyx::assets
