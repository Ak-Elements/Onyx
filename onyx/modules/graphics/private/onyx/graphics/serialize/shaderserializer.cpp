#include <onyx/graphics/serialize/shaderserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/rhi/graphicssystem.h>

namespace onyx::graphics
{
    bool ShaderSerializer::Serialize(const assets::AssetHandle<assets::AssetInterface>& /*asset*/, const assets::AssetMetaData& /*meta*/, Serializer& /*serializer*/, const IEngine& /*engine*/) const
    {
        ONYX_ASSERT(false, "Shaders are not saved from the engine.");
        return false;
    }

    bool ShaderSerializer::Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& /*deserializer*/, IEngine& engine) const
    {
        rhi::GraphicsSystem& graphicsSystem = engine.GetSystem<rhi::GraphicsSystem>();

        Reference<rhi::Shader> shaderRef(asset.GetHandle().Raw());

        rhi::ShaderCache& shaderCache = graphicsSystem.GetShaderCache();
        shaderCache.GetOrLoadShader(meta.Path, shaderRef);

        return asset.IsValid();
    }
}
