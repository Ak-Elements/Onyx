#include <onyx/volume/serialize/volumeshadergraphserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/volume/shader/generators/templates/volumeshadertemplates.h>

namespace Onyx::Volume
{
    namespace
    {
        constexpr StringView BUILD_OCTREE_SHADER_FILENAME = "build_octree.oshader";
        constexpr StringView FIND_OCTREE_NODE_SHADER_FILENAME = "find_octree_node.oshader";
        constexpr StringView GENERATE_VOLUME_MESH_SHADER_FILENAME = "generate_volume.oshader";
        constexpr StringView RAYTRACE_TERRAIN_SHADER_FILENAME = "raytrace_terrain.oshader";

        bool WriteFile(const FileSystem::Filepath& path, StringView content)
        {
            FileSystem::FileStream outFileStream(path, FileSystem::OpenMode::Write | FileSystem::OpenMode::Text);

            if (outFileStream.IsValid() == false)
                return false;

            outFileStream.WriteRaw(content.data(), content.size());
            return true;
        }

        bool WriteTemplateFile(const FileSystem::Filepath& path, StringView templateCode, FileSystem::Filepath volumeHeaderFileName)
        {
            String shaderCode = Replace(templateCode, "@VERSION@", "1");
            shaderCode = Replace(shaderCode, "@BASE_TERRAIN_SDF_SHADER@", volumeHeaderFileName.generic_string());
            WriteFile(path, shaderCode);
            return true;
        }
    }

    bool VolumeShaderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, const IEngine& /*engine*/) const
    {
        const VolumeShaderGraph& shaderGraph = asset.As<VolumeShaderGraph>();
        if (Graphics::ShaderGraphSerializer::Serialize(shaderGraph, serializer) == false)
            return false;

        FileSystem::Filepath volumeShaderPath= FileSystem::Path::ReplaceExtension(meta.Path, "h");
        FileSystem::Filepath volumeShaderGraphHeaderPath = FileSystem::Path::GetFullPath(volumeShaderPath);

        // write out header
        WriteFile(volumeShaderGraphHeaderPath, shaderGraph.GetShaderCode());

        FileSystem::Filepath directoryPath = volumeShaderGraphHeaderPath.parent_path();
        FileSystem::Filepath volumeHeaderFileName = volumeShaderPath.filename();

        WriteTemplateFile(directoryPath / BUILD_OCTREE_SHADER_FILENAME, BUILD_OCTREE_SHADER, volumeHeaderFileName);
        WriteTemplateFile(directoryPath / FIND_OCTREE_NODE_SHADER_FILENAME, FIND_OCTREE_NODE_SHADER, volumeHeaderFileName);
        WriteTemplateFile(directoryPath / GENERATE_VOLUME_MESH_SHADER_FILENAME, GENERATE_VOLUME_MESH_SHADER, volumeHeaderFileName);
        WriteTemplateFile(directoryPath / RAYTRACE_TERRAIN_SHADER_FILENAME, RAYTRACE_TERRAIN_SHADER, volumeHeaderFileName);

        return true;
    }

    bool VolumeShaderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        VolumeShaderGraph& shaderGraph = asset.As<VolumeShaderGraph>();
        if (Graphics::ShaderGraphSerializer::Deserialize(shaderGraph, deserializer) == false)
            return false;

        FileSystem::Filepath directoryPath = FileSystem::Path::ConvertToMountPath(meta.Path).parent_path();
        shaderGraph.m_BuildOctreeShader = Assets::AssetId(directoryPath / BUILD_OCTREE_SHADER_FILENAME);
        shaderGraph.m_FindOctreeNodeShader = Assets::AssetId(directoryPath / FIND_OCTREE_NODE_SHADER_FILENAME);
        shaderGraph.m_GenerateVolumeMeshShader = Assets::AssetId(directoryPath / GENERATE_VOLUME_MESH_SHADER_FILENAME);
        shaderGraph.m_RaytraceTerrainShader = Assets::AssetId(directoryPath / RAYTRACE_TERRAIN_SHADER_FILENAME);

        return true;
    }

}
