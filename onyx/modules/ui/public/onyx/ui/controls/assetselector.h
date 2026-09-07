#pragma once

#include <onyx/assets/asset.h>
#include <onyx/assets/assetmetadata.h>
#if ONYX_IS_EDITOR
namespace onyx::assets {
class AssetSystem;
} // namespace onyx::assets

namespace onyx::ui {
bool assetSelector( assets::AssetSystem& assetSystem, assets::AssetType assetType, assets::AssetId& outAssetId );

template < typename T > requires std::is_base_of_v< assets::AssetInterface, T >
bool assetSelector( assets::AssetSystem& assetSystem, assets::AssetHandle< T >& outAsset ) {
    assets::AssetId id = outAsset.getId();
    constexpr assets::AssetType Type = enums::toEnum< assets::AssetType >( T::TypeId.getId() );
    if( assetSelector( assetSystem, Type, id ) ) {
        outAsset.setId( id );
        return true;
    }

    return false;
}

} // namespace onyx::ui
#endif
