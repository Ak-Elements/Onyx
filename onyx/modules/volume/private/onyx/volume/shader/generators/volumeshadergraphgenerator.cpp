#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>

namespace onyx::volume {
VolumeShaderGraphGenerator::VolumeShaderGraphGenerator() {
    AddInclude( "includes/volume/volumesources.h" );
}

String VolumeShaderGraphGenerator::GenerateShader() {
    // TODO: I think this is not needed as a custom include anymore, use default ShaderGenerator include generation
    String includes;
    for( const String& include : m_ShaderIncludes ) {
        includes += format::format( "#include \"{}\"\n", include );
    }

    // TODO: currently all node generated code is added as fragment code
    return format::format( "{}\nvec4 SampleBaseTerrainValue(vec3 worldPosition)\n{{\n{}\n}}\n",
                           includes,
                           m_ShaderStagesCode[ enums::toIntegral( rhi::ShaderStage::Fragment ) ] );
}
} // namespace onyx::volume
