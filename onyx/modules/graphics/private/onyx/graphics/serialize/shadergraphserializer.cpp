#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>

#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template <>
    struct Serialization<Graphics::ShaderGraph>
    {
        static bool Serialize(Serializer& serializer, const Graphics::ShaderGraph& nodeGraphNode)
        {
            return nodeGraphNode.Serialize(serializer);
        }

        static bool Deserialize(const Deserializer& deserializer, Graphics::ShaderGraph& outGraphNode)
        {
            return outGraphNode.Deserialize(deserializer);
        }
    };
}

namespace Onyx::Graphics::ShaderGraphSerializerUtil
{
    bool Serialize(const ShaderGraph& graph, Serializer& serializer)
    {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        return serializer.Write(graph);
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(serializer);
        return true;
#endif

    }

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    bool Serialize(const ShaderGraph& graph, Serializer& serializer, const FileSystem::Filepath& shaderPath)
    {
        bool success = serializer.Write(graph);
        if (success == false)
        {
            return false;
        }

        FileSystem::OnyxFile shaderOutFile(FileSystem::Path::GetFullPath(shaderPath));
        FileSystem::FileStream shaderOutStream = shaderOutFile.OpenStream(FileSystem::OpenMode::Write | FileSystem::OpenMode::Text);
        shaderOutStream.WriteRaw(graph.GetShaderCode().data(), graph.GetShaderCode().size());

        return true;
    }
#endif

    bool Deserialize(ShaderGraph& graph, const Deserializer& deserializer)
    {
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        return deserializer.Read(graph);
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(deserializer);
        return true;
#endif
    }
}

namespace Onyx::Graphics
{
    bool ShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, IEngine& /*engine*/) const
    {
        const ShaderGraph& shaderGraph = asset.As<ShaderGraph>();
        FileSystem::Filepath shaderFilePath = FileSystem::Path::GetFullPath(FileSystem::Path::ReplaceExtension(meta.Path, "oshader"));
        if (ShaderGraphSerializerUtil::Serialize(shaderGraph, serializer, shaderFilePath) == false)
            return false;

        return true;
    }

    bool ShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        //GraphicsSystem& graphicsSystem = engine.GetSystem<GraphicsSystem>();
        //Assets::AssetSystem& assetSystem = engine.GetSystem<Assets::AssetSystem>();

        ShaderGraph& shaderGraph = asset.As<ShaderGraph>();

        if (ShaderGraphSerializerUtil::Deserialize(shaderGraph, deserializer) == false)
            return false;

        //TODO: load the actual shader
        return true;
    }
}