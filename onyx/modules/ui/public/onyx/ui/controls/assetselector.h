#pragma once

#if ONYX_IS_EDITOR
namespace onyx::assets
{
    struct AssetId;
    enum class AssetType : onyxU32;
    class AssetSystem;
}

namespace onyx::ui
{
    bool AssetSelector(const assets::AssetSystem& assetSystem, assets::AssetType assetType, assets::AssetId& outAssetId);
}
#endif