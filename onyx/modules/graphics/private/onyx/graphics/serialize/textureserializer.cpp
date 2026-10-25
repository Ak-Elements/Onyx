#include <onyx/graphics/serialize/textureserializer.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/rhi/graphicssystem.h>
#include <onyx/rhi/memoryaccess.h>
#include <onyx/rhi/textureproperties.h>
#include <onyx/rhi/texturestorageproperties.h>

#include <onyx/filesystem/imagefile.h>
#include <onyx/graphics/textureasset.h>

namespace onyx::graphics {
bool TextureSerializer::serialize( [[maybe_unused]] const assets::AssetHandle< assets::AssetInterface >& asset,
                                   [[maybe_unused]] const assets::AssetMetaData& meta,
                                   [[maybe_unused]] Serializer& serializer,
                                   [[maybe_unused]] const IEngine& engine ) const {
    return false;
}

bool TextureSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                     const assets::AssetMetaData& meta,
                                     const Deserializer& /*deserializer*/,
                                     IEngine& engine ) const {
    // TODO: Fix texture serializer for different formats (srgb etc)
    rhi::GraphicsSystem& graphicsSystem = engine.getSystem< rhi::GraphicsSystem >();
    TextureAsset& textureAsset = asset.as< TextureAsset >();

    file_system::ImageFile file( file_system::path::replaceExtension( meta.Path, "png" ) );

    rhi::TextureStorageProperties storageProps;
    if( file.getChannelCount() == 1 ) {
        storageProps.Format = file.getBitsPerChannel() == 8 ? rhi::TextureFormat::R_UNORM8
                                                            : rhi::TextureFormat::R_UNORM16;
    } else if( file.getChannelCount() == 2 ) {
        storageProps.Format = rhi::TextureFormat::RG_UNORM8;
    } else {
        storageProps.Format = file.getBitsPerChannel() == 8 ? rhi::TextureFormat::RGBA_UNORM8
                                                            : rhi::TextureFormat::RGBA_UNORM16;

        if( ignoreCaseFind( meta.Path.generic_string(), "basecolor" ) != StringView::npos ) {
            storageProps.Format = rhi::TextureFormat::SRGB_UNORM8;
        }
    }

    const Vector2s32& imageExtents = file.getSize();
    bool isNormalTexture = ignoreCaseFind( meta.Path.generic_string(), "normal" ) != StringView::npos;
    storageProps.Type = rhi::TextureType::Texture2D;
    storageProps.Size = { imageExtents.X, imageExtents.Y, 1 };

    uint32_t mipLevels = static_cast< uint32_t >(
                             std::floor( std::log2( std::max( storageProps.Size.X, storageProps.Size.Y ) ) ) ) +
                         1;
    storageProps.MaxMipLevel = mipLevels;

    storageProps.ArraySize = 0;
    storageProps.MsaaProperties = { 1, 1 }; // samples /quality
    storageProps.CpuAccess = rhi::CPUAccess::None;
    storageProps.GpuAccess = rhi::GPUAccess::Read;
    storageProps.IsTexture = true;
    storageProps.IsNormalTexture = isNormalTexture;
    storageProps.DebugName = format::format( "{} Texture Storage", meta.getName() );

    rhi::TextureProperties textureProps;
    textureProps.Format = storageProps.Format;
    textureProps.DebugName = format::format( "{} Texture", meta.getName() );
    const Span< uint8_t >& imageData = file.getData();

    graphicsSystem.createTexture( textureAsset.m_texture, storageProps, textureProps, imageData );

    // // TODO: Rewrite mip generation to happen on texture import and save mip chain to disk
    // if( isNormalTexture ) {
    //     if( g_mipMapGenerationShader == rhi::ShaderInstanceHandle::invalid() ) {
    //         g_mipMapGenerationShader = graphicsSystem.createShaderInstance(
    //             "engine:/shaders/compute/generatemips.slang" );
    //
    //         while( g_mipMapGenerationShader.isValid() == false ) {
    //             std::this_thread::yield();
    //         }
    //
    //         generateMips();
    //     } else {
    //         generateMips();
    //     }
    // }

    return true;
}

} // namespace onyx::graphics
