#pragma once

#include <onyx/filesystem/path.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    class GraphicsApi;
    class ShaderGraph;

    namespace ShaderGraphSerializer
    {
        bool SerializeJson(const ShaderGraph& graph, const FileSystem::Filepath& filePath);
        bool DeserializeJson(ShaderGraph& graph, GraphicsApi& graphicsApi, Assets::AssetSystem& assetSystem, const FileSystem::Filepath& filePath);
    };
}
