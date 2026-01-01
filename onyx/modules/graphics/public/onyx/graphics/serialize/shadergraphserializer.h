#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/path.h>

namespace Onyx::Assets
{
    struct AssetMetaData;
}

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    class GraphicsSystem;
    class ShaderGraph;

    namespace ShaderGraphSerializerUtil
    {
        bool Serialize(const ShaderGraph& graph, Serializer& serializer);

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        bool Serialize(const ShaderGraph& graph, Serializer& serializer, const FileSystem::Filepath& shaderPath);
#endif

        bool Deserialize(ShaderGraph& graph, const Deserializer& deserializer);
    }

    struct ShaderGraphSerializer : Assets::AssetSerializer<ShaderGraph>
    {
        static constexpr Array<StringView, 1> Extensions{ "oshadergraph" };

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
    };
}
