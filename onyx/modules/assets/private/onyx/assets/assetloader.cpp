#include <onyx/assets/assetloader.h>

namespace onyx::assets
{
    void AssetIOHandler::OnAssetLoadFinished(AssetHandle<AssetInterface>& handle)
    {
        m_LoadRequests.erase(handle.GetId());
    }

#if ONYX_IS_EDITOR
    void AssetIOHandler::OnAssetSaveFinished(const AssetHandle<AssetInterface>& handle)
    {
        m_SaveRequests.erase(handle.GetId());
    }
#endif
}
