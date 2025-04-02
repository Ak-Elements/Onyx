#include <onyx/assets/assetloader.h>

namespace Onyx::Assets
{
    void AssetIOHandler::OnAssetLoadFinished(Reference<AssetInterface>& handle)
    {
        m_LoadRequests.erase(handle->GetId());
    }

#if ONYX_IS_EDITOR
    void AssetIOHandler::OnAssetSaveFinished(const Reference<AssetInterface>& handle)
    {
        m_SaveRequests.erase(handle->GetId());
    }
#endif
}
