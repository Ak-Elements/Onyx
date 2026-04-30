#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shaderserializer.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::graphics {
bool ShaderSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& /*asset*/,
                                  const assets::AssetMetaData& /*meta*/,
                                  Serializer& /*serializer*/,
                                  const IEngine& /*engine*/ ) const {
    ONYX_ASSERT( false, "Shaders are not saved from the engine." );
    return false;
}

bool ShaderSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                    const assets::AssetMetaData& meta,
                                    const Deserializer& /*deserializer*/,
                                    IEngine& engine ) const {
    rhi::GraphicsSystem& graphicsSystem = engine.getSystem< rhi::GraphicsSystem >();

    Reference< rhi::Shader > shaderRef( asset.getHandle().raw() );

    rhi::ShaderCache& shaderCache = graphicsSystem.getShaderCache();
    shaderCache.getOrLoadShader( meta.Path, shaderRef );

    return asset.isValid();
}
} // namespace onyx::graphics
