#include <onyx/volume/serialize/volumeshadergraphserializer.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/engine/enginesystem.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/serialize/shadergraphserializer.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

#include <onyx/volume/shader/generators/templates/volumeshadertemplates.h>

namespace onyx::volume {
namespace {
constexpr StringView RenderTerrainShaderFilename = "render_terrain.slang";

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

    FilePath volumeShaderPath = file_system::path::replaceExtension( meta.Path, "slang" );
    FilePath volumeShaderGraphHeaderPath = file_system::path::getFullPath( volumeShaderPath );

    // write out header
    writeFile( volumeShaderGraphHeaderPath, shaderGraph.getShaderCode() );

    FilePath directoryPath = volumeShaderGraphHeaderPath.parent_path();
    FilePath volumeHeaderFileName = volumeShaderPath.stem();

    writeTemplateFile( directoryPath / RenderTerrainShaderFilename, RenderTerrainShader, volumeHeaderFileName );

    return true;
}

bool VolumeShaderGraphSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                               const assets::AssetMetaData& meta,
                                               const Deserializer& deserializer,
                                               IEngine& engine ) const {
    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();

    VolumeShaderGraph& shaderGraph = asset.as< VolumeShaderGraph >();

    if( graphics::shader_graph_serializer::deserialize( shaderGraph, deserializer ) == false )
        return false;

    shaderGraph.getNodeGraph().compile();
    FilePath directoryPath = file_system::path::convertToMountPath( meta.Path ).parent_path();
    shaderGraph.m_renderTerrainShader = assetSystem.resolveAssetId( directoryPath / RenderTerrainShaderFilename );

    return true;
}

} // namespace onyx::volume
