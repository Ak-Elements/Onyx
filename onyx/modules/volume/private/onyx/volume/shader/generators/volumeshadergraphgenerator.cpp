#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>

namespace Onyx::Volume
{
    VolumeShaderGraphGenerator::VolumeShaderGraphGenerator()
    {
        AddInclude(Graphics::ShaderStage::All, "includes/volume/volumesources.h");
    }

    String VolumeShaderGraphGenerator::GenerateShader()
    {
        String includes;
        for (const String& include : shaderStagesIncludes[Enums::ToIntegral(Graphics::ShaderStage::All)])
        {
            includes += Format::Format("#include \"{}\"\n", include);
        }

        // TODO: currently all node generated code is added as fragment code
        return Format::Format("{}\nvec4 SampleBaseTerrainValue(vec3 worldPosition)\n{{\n{}\n}}\n", Graphics::ShaderCoreVersion, includes, shaderStagesCode[Enums::ToIntegral(Graphics::ShaderStage::Fragment)]);
    }
}
