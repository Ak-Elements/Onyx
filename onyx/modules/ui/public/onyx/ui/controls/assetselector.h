#pragma once

#if ONYX_IS_EDITOR
namespace onyx::assets {
struct AssetId;
enum class AssetType : uint32_t;
class AssetSystem;
} // namespace onyx::assets

namespace onyx::ui {
bool AssetSelector( const assets::AssetSystem& assetSystem, assets::AssetType assetType, assets::AssetId& outAssetId );
}
#endif