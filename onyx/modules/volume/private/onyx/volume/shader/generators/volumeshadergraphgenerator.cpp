#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>

namespace onyx::volume {

VolumeShaderGraphGenerator::VolumeShaderGraphGenerator()
    : ShaderGenerator( rhi::ShaderStage::All ) {
    addImport( "includes.volume.terrainsample" );
    addImport( "includes.volume.volumesource" );
}

void VolumeShaderGraphGenerator::generateShaderStage( rhi::ShaderGenerator::StageGenerationContext& stageContext ) {
    if( stageContext.Stage == rhi::ShaderStage::All ) {
        stageContext.Code = format::format(
            "public export SdfSample sampleBaseTerrainValue( float3 worldPosition, "
            "uint32_t heightTextureIndex, float maxHeightDisplacement, float heightTextureScale ) {{\n{}\n }}",
            stageContext.Code );
    }
}

} // namespace onyx::volume
