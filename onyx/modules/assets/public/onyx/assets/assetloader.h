#pragma once
#include <onyx/assets/assetloadrequest.h>
#include <onyx/assets/assetserializer.h>

#include <onyx/thread/async/asynctask.h>
#include <onyx/thread/threadpool/threadpool.h>

#include <onyx/assets/assethandle.h>

namespace onyx::assets {
struct AssetMetaData;
class AssetInterface;

class AssetIOHandler {
  public:
    void requestLoad( const AssetMetaData& metaData,
                      const AssetHandle< AssetInterface >& assetHandle,
                      const UniquePtr< IAssetSerializer >& serializer,
                      IEngine* engine );

#if ONYX_IS_EDITOR
    void requestSave( const AssetMetaData& metaData,
                      const AssetHandle< AssetInterface >& assetHandle,
                      const UniquePtr< IAssetSerializer >& serializer,
                      const IEngine* engine );
#endif

  private:
    void onAssetLoadFinished( AssetHandle< AssetInterface >& handle );
#if ONYX_IS_EDITOR
    void onAssetSaveFinished( const AssetHandle< AssetInterface >& handle );
#endif
  private:
    // default initalize to the amount of logical cores available
    HashMap< AssetId, UniquePtr< AssetLoadRequest > > m_loadRequests;
#if ONYX_IS_EDITOR
    HashMap< AssetId, UniquePtr< AssetSaveRequest > > m_saveRequests;
#endif

#if ONYX_PROFILER_ENABLED
    threading::ThreadPool m_loaderThreadPool{ threading::ThreadPoolOptions(), "Asset Loader" };
#else
    threading::ThreadPool m_loaderThreadPool{ threading::ThreadPoolOptions( 1 ) };
#endif
};
} // namespace onyx::assets
