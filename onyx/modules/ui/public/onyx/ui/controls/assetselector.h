#pragma once

#if ONYX_IS_EDITOR
namespace Onyx::Assets
{
    struct AssetId;
    enum class AssetType : onyxU32;
    class AssetSystem;
}

namespace Onyx::Ui
{
    bool AssetSelector(const Assets::AssetSystem& assetSystem, Assets::AssetType assetType, Assets::AssetId& outAssetId);
}
#endif