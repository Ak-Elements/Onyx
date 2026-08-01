#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>

namespace onyx::volume {
VolumeShaderGraphGenerator::VolumeShaderGraphGenerator() {
    addInclude( "includes/volume/volumesources.h" );
}

String VolumeShaderGraphGenerator::generateShader() {
    // TODO: I think this is not needed as a custom include anymore, use default ShaderGenerator include generation
    String includes;
    for( const String& include : m_shaderIncludes ) {
        includes += format::format( "#include \"{}\"\n", include );
    }

    // TODO: currently all node generated code is added as fragment code
    return format::format( "{}\nvec4 SampleBaseTerrainValue(vec3 worldPosition)\n{{\n{}\n}}\n",
                           includes,
                           m_shaderStagesCode[ enums::toIntegral( rhi::ShaderStage::Fragment ) ] );
}
} // namespace onyx::volume
