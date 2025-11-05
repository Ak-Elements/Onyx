#include <onyx/graphics/serialize/shaderserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/graphicssystem.h>

namespace Onyx::Graphics
{
    ShaderSerializer::ShaderSerializer(Assets::AssetSystem& assetSystem, GraphicsSystem& graphicsSystem)
        : AssetSerializer(assetSystem)
        , m_GraphicsSystem(&graphicsSystem)
    {
    }

    bool ShaderSerializer::Serialize(const Reference<Assets::AssetInterface>& /*asset*/, const Assets::AssetMetaData& /*meta*/, Serializer& /*serializer*/) const
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);
        ONYX_ASSERT(false, "Shaders are not saved from the engine.");
        return false;
    }

    bool ShaderSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& /*deserializer*/) const
    {
        ONYX_ASSERT(m_GraphicsSystem != nullptr);

        ShaderHandle shaderHandle(asset.Raw());

        ShaderCache& shaderCache = m_GraphicsSystem->GetShaderCache();
        shaderCache.GetOrLoadShader(meta.Path, shaderHandle);

        //m_GraphicsSystem->GetShader(shaderHandle, meta.Path);
        return asset.IsValid();
    }
}
