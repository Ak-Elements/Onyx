#include <onyx/assets/assetloader.h>

namespace onyx::assets {

void AssetIOHandler::requestLoad( const AssetMetaData& metaData,
                                  const AssetHandle< AssetInterface >& assetHandle,
                                  const UniquePtr< IAssetSerializer >& serializer,
                                  IEngine* engine ) {
    if ( m_loadRequests.contains( metaData.Id ) )
        return;

    UniquePtr< AssetLoadRequest > loadRequest = makeUnique< AssetLoadRequest >();
    loadRequest->Engine = engine;
    loadRequest->MetaData = metaData;
    loadRequest->Asset = assetHandle;
    loadRequest->Serializer = serializer.get();
    loadRequest->OnLoadFinished.Connect< &AssetIOHandler::onAssetLoadFinished >( this );
    loadRequest->start( m_loaderThreadPool );

    m_loadRequests[ metaData.Id ] = std::move( loadRequest );
}

void AssetIOHandler::onAssetLoadFinished( AssetHandle< AssetInterface >& handle ) {
    m_loadRequests.erase( handle.getId() );
}

#if ONYX_IS_EDITOR
void AssetIOHandler::requestSave( const AssetMetaData& metaData,
                                  const AssetHandle< AssetInterface >& assetHandle,
                                  const UniquePtr< IAssetSerializer >& serializer,
                                  const IEngine* engine ) {
    if ( m_saveRequests.contains( metaData.Id ) )
        return;

    UniquePtr< AssetSaveRequest > saveRequest = makeUnique< AssetSaveRequest >();
    saveRequest->Engine = engine;
    saveRequest->MetaData = metaData;
    saveRequest->Asset = assetHandle;
    saveRequest->Serializer = serializer.get();
    saveRequest->OnSaveFinished.Connect< &AssetIOHandler::onAssetSaveFinished >( this );
    saveRequest->start( m_loaderThreadPool );

    m_saveRequests[ metaData.Id ] = std::move( saveRequest );
}

void AssetIOHandler::onAssetSaveFinished( const AssetHandle< AssetInterface >& handle ) {
    m_saveRequests.erase( handle.getId() );
}
#endif
} // namespace onyx::assets
