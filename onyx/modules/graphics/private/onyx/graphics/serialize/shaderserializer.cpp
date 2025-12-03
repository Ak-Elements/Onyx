#include <onyx/graphics/serialize/shaderserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicssystem.h>

namespace Onyx::Graphics
{
    bool ShaderSerializer::Serialize(const Reference<Assets::AssetInterface>& /*asset*/, const Assets::AssetMetaData& /*meta*/, Serializer& /*serializer*/, const IEngine& /*engine*/) const
    {
        ONYX_ASSERT(false, "Shaders are not saved from the engine.");
        return false;
    }

    bool ShaderSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& /*deserializer*/, IEngine& engine) const
    {
        GraphicsSystem& graphicsSystem = engine.GetSystem<GraphicsSystem>();

        ShaderHandle shaderHandle(asset.Raw());

        ShaderCache& shaderCache = graphicsSystem.GetShaderCache();
        shaderCache.GetOrLoadShader(meta.Path, shaderHandle);

        return asset.IsValid();
    }
}
