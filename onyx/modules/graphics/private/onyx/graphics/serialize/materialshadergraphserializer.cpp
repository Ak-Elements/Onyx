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

    bool MaterialShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return false;
    }

    bool MaterialShaderGraphSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        const MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();
        return ShaderGraphSerializer::SerializeJson(shaderGraph, filePath);
    }

    bool MaterialShaderGraphSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return false;
    }

    bool MaterialShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return false;
    }

    bool MaterialShaderGraphSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        MaterialShaderGraph& shaderGraph = asset.As<MaterialShaderGraph>();
        return ShaderGraphSerializer::DeserializeJson(shaderGraph, graphicsApi, m_AssetSystem, filePath);
    }

    bool MaterialShaderGraphSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return false;
    }
}
