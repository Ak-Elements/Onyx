#include <onyx/graphics/serialize/textureserializer.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/memoryaccess.h>
#include <onyx/rhi/textureproperties.h>
#include <onyx/rhi/texturestorageproperties.h>

#include <onyx/filesystem/imagefile.h>
#include <onyx/graphics/textureasset.h>

namespace onyx::graphics {
bool TextureSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                   const assets::AssetMetaData& meta,
                                   Serializer& serializer,
                                   const IEngine& /*engine*/ ) const {
    ONYX_UNUSED( asset );
    ONYX_UNUSED( meta );
    ONYX_UNUSED( serializer );
    return false;
}

bool TextureSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                     const assets::AssetMetaData& meta,
                                     const Deserializer& /*deserializer*/,
                                     IEngine& engine ) const {
    rhi::GraphicsSystem& graphicsSystem = engine.getSystem< rhi::GraphicsSystem >();
    TextureAsset& textureAsset = asset.as< TextureAsset >();

    file_system::ImageFile file( file_system::path::replaceExtension( meta.Path, "png" ) );

    rhi::TextureStorageProperties storageProps;
    storageProps.m_Format = /*(file.GetChannelCount() == 4) ? */ rhi::TextureFormat::
        RGBA_UNORM8; // : graphics::TextureFormat::RGB_UNORM8;
    storageProps.m_Type = rhi::TextureType::Texture2D;
    storageProps.m_Size = { file.GetSize()[ 0 ], file.GetSize()[ 1 ], 1 };
    storageProps.m_MaxMipLevel = 1;
    storageProps.m_ArraySize = 0;
    storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality
    storageProps.m_CpuAccess = rhi::CPUAccess::None;
    storageProps.m_GpuAccess = rhi::GPUAccess::Read;
    storageProps.m_IsTexture = true;
    storageProps.m_DebugName = format::format( "{} Texture Storage", meta.getName() );

    rhi::TextureProperties textureProps;
    textureProps.m_Format = storageProps.m_Format;
    textureProps.m_DebugName = format::format( "{} Texture", meta.getName() );
    const Span< uint8_t >& imageData = file.GetData();

    graphicsSystem.createTexture( textureAsset.m_Texture, storageProps, textureProps, imageData );
    return true;
}
} // namespace onyx::graphics
