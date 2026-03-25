#include <onyx/volume/serialize/volumeshadergraphserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

#include <onyx/volume/shader/generators/templates/volumeshadertemplates.h>

namespace onyx::volume {
namespace {
constexpr StringView BUILD_OCTREE_SHADER_FILENAME = "build_octree.oshader";
constexpr StringView FIND_OCTREE_NODE_SHADER_FILENAME = "find_octree_node.oshader";
constexpr StringView GENERATE_VOLUME_MESH_SHADER_FILENAME = "generate_volume.oshader";
constexpr StringView RAYTRACE_TERRAIN_SHADER_FILENAME = "raytrace_terrain.oshader";

bool WriteFile( const FilePath& path, StringView content ) {
    file_system::FileStream outFileStream( path, file_system::OpenMode::Write | file_system::OpenMode::Text );

    if ( outFileStream.isValid() == false )
        return false;

    outFileStream.writeRaw( content.data(), content.size() );
    return true;
}

bool WriteTemplateFile( const FilePath& path, StringView templateCode, FilePath volumeHeaderFileName ) {
    String shaderCode = replace( templateCode, "@VERSION@", "1" );
    shaderCode = replace( shaderCode, "@BASE_TERRAIN_SDF_SHADER@", volumeHeaderFileName.generic_string() );
    WriteFile( path, shaderCode );
    return true;
}
} // namespace

bool VolumeShaderGraphSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                             const assets::AssetMetaData& meta,
                                             Serializer& serializer,
                                             const IEngine& /*engine*/ ) const {
    const VolumeShaderGraph& shaderGraph = asset.as< VolumeShaderGraph >();
    if ( graphics::shader_graph_serializer::serialize( shaderGraph, serializer ) == false )
        return false;

    FilePath volumeShaderPath = file_system::Path::ReplaceExtension( meta.Path, "h" );
    FilePath volumeShaderGraphHeaderPath = file_system::Path::GetFullPath( volumeShaderPath );

    // write out header
    WriteFile( volumeShaderGraphHeaderPath, shaderGraph.GetShaderCode() );

    FilePath directoryPath = volumeShaderGraphHeaderPath.parent_path();
    FilePath volumeHeaderFileName = volumeShaderPath.filename();

    WriteTemplateFile( directoryPath / BUILD_OCTREE_SHADER_FILENAME, BUILD_OCTREE_SHADER, volumeHeaderFileName );
    WriteTemplateFile( directoryPath / FIND_OCTREE_NODE_SHADER_FILENAME,
                       FIND_OCTREE_NODE_SHADER,
                       volumeHeaderFileName );
    WriteTemplateFile( directoryPath / GENERATE_VOLUME_MESH_SHADER_FILENAME,
                       GENERATE_VOLUME_MESH_SHADER,
                       volumeHeaderFileName );
    WriteTemplateFile( directoryPath / RAYTRACE_TERRAIN_SHADER_FILENAME,
                       RAYTRACE_TERRAIN_SHADER,
                       volumeHeaderFileName );

    return true;
}

bool VolumeShaderGraphSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                               const assets::AssetMetaData& meta,
                                               const Deserializer& deserializer,
                                               IEngine& /*engine*/ ) const {
    VolumeShaderGraph& shaderGraph = asset.as< VolumeShaderGraph >();
    if ( graphics::shader_graph_serializer::deserialize( shaderGraph, deserializer ) == false )
        return false;

    FilePath directoryPath = file_system::Path::ConvertToMountPath( meta.Path ).parent_path();
    shaderGraph.m_BuildOctreeShader = assets::AssetId( directoryPath / BUILD_OCTREE_SHADER_FILENAME );
    shaderGraph.m_FindOctreeNodeShader = assets::AssetId( directoryPath / FIND_OCTREE_NODE_SHADER_FILENAME );
    shaderGraph.m_GenerateVolumeMeshShader = assets::AssetId( directoryPath / GENERATE_VOLUME_MESH_SHADER_FILENAME );
    shaderGraph.m_RaytraceTerrainShader = assets::AssetId( directoryPath / RAYTRACE_TERRAIN_SHADER_FILENAME );

    return true;
}

} // namespace onyx::volume
