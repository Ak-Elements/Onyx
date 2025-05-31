#include <onyx/graphics/serialize/materialshadergraphserializer.h>

#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

namespace Onyx::Graphics
{
    MaterialShaderGraphSerializer::MaterialShaderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , graphicsApi(graphicsApi)
    {
    }

    bool MaterialShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const
    {
        const MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();
        return ShaderGraphSerializer::Serialize(shaderGraph, meta, serializer);
    }

    bool MaterialShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const
    {
        MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();
        return ShaderGraphSerializer::Deserialize(shaderGraph, graphicsApi, m_AssetSystem, meta, deserializer);
    }

}
