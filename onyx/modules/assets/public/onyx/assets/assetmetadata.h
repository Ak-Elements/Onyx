#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/function/signal.h>

#include <onyx/assets/assetformat.h>
#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>

#include <onyx/serialize/serialization.h>

namespace onyx {

namespace assets {
// AssetType is the hash of the asset class to use. e.g.: AudioAsset, MeshAsset, StaticMeshAsset, ...
// class hashes are constructed using entt type_info
// NOLINTNEXTLINE
enum class AssetType : uint32_t { Invalid };

struct AssetMetaData {
    FilePath Path;
    AssetId Id;
    AssetType Type = AssetType::Invalid;

    AssetFormat Format = AssetFormat::Binary;

    int64_t Handle = InvalidIndex64;

    uint32_t Version = 0; // Maybe not needed

    [[nodiscard]] String getName() const { return Path.stem().string(); }

    [[nodiscard]] String getExtension() const {
        // TODO: remove extension once we have meta data stored on disk
        String extension = Path.extension().string();
        if( extension.empty() == false ) {
            return extension.substr( 1 ); // ignore .
        }

        return "";
    }
};

} // namespace assets

template <>
struct Serialization< onyx::assets::AssetMetaData > {
    static bool serialize( Serializer& serializer, const onyx::assets::AssetMetaData& metadata );
    static bool deserialize( const Deserializer& deserializer, onyx::assets::AssetMetaData& outMetadata );
};

} // namespace onyx
