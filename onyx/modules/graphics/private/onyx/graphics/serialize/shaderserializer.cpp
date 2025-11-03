#include <onyx/graphics/serialize/shaderserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    ShaderSerializer::ShaderSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , m_GraphicsApi(&graphicsApi)
    {
    }

    bool ShaderSerializer::Serialize(const Reference<Assets::AssetInterface>& /*asset*/, const Assets::AssetMetaData& /*meta*/, Serializer& /*serializer*/) const
    {
        ONYX_ASSERT(m_GraphicsApi != nullptr);
        ONYX_ASSERT(false, "Shaders are not saved from the engine.");
        return false;
    }

    bool ShaderSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& /*deserializer*/) const
    {
        ONYX_ASSERT(m_GraphicsApi != nullptr);

        ShaderHandle shaderHandle(asset.Raw());

        ShaderCache& shaderCache = m_GraphicsApi->GetShaderCache();
        shaderCache.GetOrLoadShader(meta.Path, shaderHandle);

        //m_GraphicsApi->GetShader(shaderHandle, meta.Path);
        return asset.IsValid();
    }
}
