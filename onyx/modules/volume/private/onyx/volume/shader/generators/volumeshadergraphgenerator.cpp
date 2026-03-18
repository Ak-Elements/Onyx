#include <onyx/volume/shader/generators/volumeshadergraphgenerator.h>

namespace onyx::volume
{
    VolumeShaderGraphGenerator::VolumeShaderGraphGenerator()
    {
        AddInclude(rhi::ShaderStage::All, "includes/volume/volumesources.h");
    }

    String VolumeShaderGraphGenerator::GenerateShader()
    {
        String includes;
        for (const String& include : m_ShaderStagesIncludes[enums::ToIntegral(rhi::ShaderStage::All)])
        {
            includes += format::Format("#include \"{}\"\n", include);
        }

        // TODO: currently all node generated code is added as fragment code
        return format::Format("{}\nvec4 SampleBaseTerrainValue(vec3 voxelPosition)\n{{\n{}\n}}\n", includes, m_ShaderStagesCode[enums::ToIntegral(rhi::ShaderStage::Fragment)]);
    }
}
