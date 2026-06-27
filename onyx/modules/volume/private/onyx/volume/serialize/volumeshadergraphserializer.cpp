#include <onyx/volume/serialize/volumeshadergraphserializer.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

#include <onyx/volume/shader/generators/templates/volumeshadertemplates.h>

namespace onyx::volume {
namespace {
constexpr StringView BuildOctreeShaderFilename = "build_octree.oshader";
constexpr StringView FindOctreeNodeShaderFilename = "find_octree_node.oshader";
constexpr StringView GenerateVolumeMeshShaderFilename = "generate_volume.oshader";
constexpr StringView RaytraceTerrainShaderFilename = "raytrace_terrain.oshader";

bool writeFile( const FilePath& path, StringView content ) {
    file_system::FileStream outFileStream( path, file_system::OpenMode::Write | file_system::OpenMode::Text );

    if( outFileStream.isValid() == false )
        return false;

    outFileStream.writeRaw( content.data(), content.size() );
    return true;
}

bool writeTemplateFile( const FilePath& path, StringView templateCode, const FilePath& volumeHeaderFileName ) {
    String shaderCode = replace( templateCode, "@VERSION@", "1" );
    shaderCode = replace( shaderCode, "@BASE_TERRAIN_SDF_SHADER@", volumeHeaderFileName.generic_string() );
    writeFile( path, shaderCode );
    return true;
}
} // namespace

bool VolumeShaderGraphSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                             const assets::AssetMetaData& meta,
                                             Serializer& serializer,
                                             const IEngine& /*engine*/ ) const {
    const VolumeShaderGraph& shaderGraph = asset.as< VolumeShaderGraph >();
    if( graphics::shader_graph_serializer::serialize( shaderGraph, serializer ) == false )
        return false;

    FilePath volumeShaderPath = file_system::path::replaceExtension( meta.Path, "h" );
    FilePath volumeShaderGraphHeaderPath = file_system::path::getFullPath( volumeShaderPath );

    // write out header
    writeFile( volumeShaderGraphHeaderPath, shaderGraph.getShaderCode() );

    FilePath directoryPath = volumeShaderGraphHeaderPath.parent_path();
    FilePath volumeHeaderFileName = volumeShaderPath.filename();

    writeTemplateFile( directoryPath / BuildOctreeShaderFilename, BuildOctreeShader, volumeHeaderFileName );
    writeTemplateFile( directoryPath / FindOctreeNodeShaderFilename, FindOctreeNodeShader, volumeHeaderFileName );
    writeTemplateFile( directoryPath / GenerateVolumeMeshShaderFilename,
                       GenerateVolumeMeshShader,
                       volumeHeaderFileName );
    writeTemplateFile( directoryPath / RaytraceTerrainShaderFilename, RaytraceTerrainShader, volumeHeaderFileName );

    return true;
}

bool VolumeShaderGraphSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                               const assets::AssetMetaData& meta,
                                               const Deserializer& deserializer,
                                               IEngine& /*engine*/ ) const {
    VolumeShaderGraph& shaderGraph = asset.as< VolumeShaderGraph >();
    if( graphics::shader_graph_serializer::deserialize( shaderGraph, deserializer ) == false )
        return false;

    FilePath directoryPath = file_system::path::convertToMountPath( meta.Path ).parent_path();
    shaderGraph.m_buildOctreeShader = assets::AssetId( directoryPath / BuildOctreeShaderFilename );
    shaderGraph.m_findOctreeNodeShader = assets::AssetId( directoryPath / FindOctreeNodeShaderFilename );
    shaderGraph.m_generateVolumeMeshShader = assets::AssetId( directoryPath / GenerateVolumeMeshShaderFilename );
    shaderGraph.m_raytraceTerrainShader = assets::AssetId( directoryPath / RaytraceTerrainShaderFilename );

    return true;
}

} // namespace onyx::volume
