#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    namespace
    {
        #if ONYX_ASSERTS_ENABLED
        constexpr onyxU8 MAX_TEXTURES = 8;
        #endif
    }

    void ShaderGenerator::GenerateVertexShader()
    {
        SetStage(ShaderStage::Vertex);

        String vertexShaderCode;

        GenerateIncludes(vertexShaderCode);
        
        m_VertexInputs.emplace_back("InPosition", ShaderDataType::Float3);
        m_VertexInputs.emplace_back("InNormal", ShaderDataType::Float3);

        m_VertexOutputs.emplace_back("WorldPosition", ShaderDataType::Float3);
        m_VertexOutputs.emplace_back("WorldNormal", ShaderDataType::Float3);

        onyxU32 locationIndex = 0;
        for (const ShaderVariable& vertexInput : m_VertexInputs)
        {
            String typeAsString = vertexInput.Type == ShaderDataType::Float3 ? "vec3" : "vec2";
            vertexShaderCode += Format::Format("layout (location = {}) in {} {};\n", locationIndex++, typeAsString, vertexInput.Name);
        }

        if (m_VertexOutputs.empty() == false)
        {
            vertexShaderCode += "struct OutStruct\n{\n";

            for (const ShaderVariable& vertexOutput : m_VertexOutputs)
            {
                String typeAsString = vertexOutput.Type == ShaderDataType::Float3 ? "vec3" : "vec2";
                vertexShaderCode += Format::Format("{} {};\n", typeAsString, vertexOutput.Name);
            }

            vertexShaderCode += "};\n";
            vertexShaderCode += "layout(location = 0) out OutStruct Output;\n";
        }

        GeneratePushConstants(vertexShaderCode);

        StringView body = "Output.WorldPosition = InPosition;\n"
            "Output.WorldNormal = InNormal;\n"
            "gl_Position = u_ViewConstants.ProjectionMatrix * u_ViewConstants.ViewMatrix * vec4(InPosition, 1.0);";

        vertexShaderCode += Format::Format("void main() \n{{ \n {} \n}}\n", body);

        // TODO: add custom vertex code if there was one
        //vertexShaderCode += Format::Format("{}\n", shaderStagesCode[Enums::ToIntegral(ShaderStage::Vertex)]);

        m_ShaderStagesCode[Enums::ToIntegral(ShaderStage::Vertex)] = vertexShaderCode;
    }

    void ShaderGenerator::AppendCode(StringView code)
    {
        String& shaderCode = m_ShaderStagesCode[Enums::ToIntegral(m_CurrentStage)];
        shaderCode.append(code);
    }

    bool ShaderGenerator::HasPushConstant(StringView name) const
    {
        return HasPushConstant(m_CurrentStage, name);
    }

    bool ShaderGenerator::HasPushConstant(ShaderStage stage, StringView name) const
    {
        return std::ranges::any_of(m_PushConstants[Enums::ToIntegral(stage)], [&](const ShaderVariable& variable) { return variable.Name == name; });
    }

    void ShaderGenerator::AddPushConstant(StringView name, ShaderDataType type)
    {
        AddPushConstant(m_CurrentStage, name, type);
    }

    void ShaderGenerator::AddPushConstant(ShaderStage stage, StringView name, ShaderDataType type)
    {
        DynamicArray<ShaderVariable>& stagePushConstants = m_PushConstants[Enums::ToIntegral(stage)];
        ONYX_ASSERT(HasPushConstant(stage, name) == false, "Push constant with that name already exists.");

        stagePushConstants.emplace_back(String(name), type);
    }

    void ShaderGenerator::AddInclude(String include)
    {
        AddInclude(m_CurrentStage, include);
    }

    void ShaderGenerator::AddInclude(ShaderStage stage, String include)
    {
        m_ShaderStagesIncludes[Enums::ToIntegral(stage)].emplace(include);
    }

    void ShaderGenerator::GeneratePushConstants(String& stageCode)
    {
        // TODO: Probably need to add padding

        const DynamicArray<ShaderVariable>& stagePushConstants = m_PushConstants[Enums::ToIntegral(m_CurrentStage)];
        if (((m_CurrentStage == ShaderStage::Vertex) && stagePushConstants.empty()) ||
            ((m_CurrentStage == ShaderStage::Fragment) && stagePushConstants.empty() && m_Textures.empty()))
        {
            return;
        }

        stageCode += "layout( push_constant ) uniform PushConstants \n";
        stageCode += "{ \n";

        for (const ShaderVariable& variable : stagePushConstants)
        {
            stageCode += Format::Format("{} {};\n", variable.Type, variable.Name);
        }

        // Check if enough space for texture indices
        if ((m_CurrentStage == ShaderStage::Fragment) && (m_Textures.empty() == false))
        {
            ONYX_ASSERT(static_cast<onyxU8>(m_Textures.size()) <= MAX_TEXTURES);
            stageCode += "uint TextureIndices[8]; \n";
        }

        stageCode += "}; \n";
    }

    void ShaderGenerator::GenerateIncludes(String& stageCode)
    {
        for (const String& include : m_ShaderStagesIncludes[Enums::ToIntegral(m_CurrentStage)])
        {
            stageCode += Format::Format("#include \"{}\"\n", include);
        }

        stageCode += "\n";
    }

    void ShaderGenerator::GenerateFragmentShader()
    {
        SetStage(ShaderStage::Fragment);

        String fragmentShaderCode;

        GenerateIncludes(fragmentShaderCode);

        if (m_VertexOutputs.empty() == false)
        {
            fragmentShaderCode += "layout(location = 0) in InStruct \n{\n";

            for (const ShaderVariable& vertexOutput : m_VertexOutputs)
            {
                String typeAsString = vertexOutput.Type == ShaderDataType::Float3 ? "vec3" : "vec2";
                fragmentShaderCode += Format::Format("{} {};\n", typeAsString, vertexOutput.Name);
            }

            fragmentShaderCode += "};\n";
        }

        fragmentShaderCode += "layout(location = 0) out vec4 outColor;\n";

        GeneratePushConstants(fragmentShaderCode);

        DoGenerateFragmentMain();
        fragmentShaderCode += Format::Format("void main() \n{{ \n {} \n}}\n", m_ShaderStagesCode[Enums::ToIntegral(ShaderStage::Fragment)]);

        m_ShaderStagesCode[Enums::ToIntegral(ShaderStage::Fragment)] = fragmentShaderCode;
    }

    String ShaderGenerator::GenerateShader()
    {
        GenerateVertexShader();
        GenerateFragmentShader();

        String commonIncludes;
        for (const String& include : m_ShaderStagesIncludes[Enums::ToIntegral(ShaderStage::All)])
        {
            commonIncludes += Format::Format("#include \"{}\"\n", include);
        }

        return Format::Format("#version 460 core\n{}\nvertex\n{{\n{}\n}} \nfragment\n{{\n{}\n}}", commonIncludes, m_ShaderStagesCode[Enums::ToIntegral(ShaderStage::Vertex)], m_ShaderStagesCode[Enums::ToIntegral(ShaderStage::Fragment)]);
    }

    PBRShaderGenerator::PBRShaderGenerator()
    {
        AddInclude(ShaderStage::All, "includes/common.h");
        AddInclude(ShaderStage::All, "includes/viewconstants.h");

        AddInclude(ShaderStage::Fragment, "includes/lighting.h");

        AddPushConstant(ShaderStage::Fragment, "LightClusterSize", ShaderDataType::UInt4);
        AddPushConstant(ShaderStage::Fragment, "LightClusterScale", ShaderDataType::Float);
        AddPushConstant(ShaderStage::Fragment, "LightClusterBias", ShaderDataType::Float);
    }

    void PBRShaderGenerator::DoGenerateFragmentMain()
    {
        AppendCode("vec3 worldPosition = WorldPosition;\n");
        AppendCode("vec3 worldNormal = WorldNormal;\n");
        AppendCode("uint clusterIndex = GetClusterIndex(gl_FragCoord, LightClusterSize, LightClusterScale, LightClusterBias);\n");
        AppendCode("outColor = vec4(CalculatePBRLighting(worldPosition, worldNormal, u_ViewConstants.CameraPosition, clusterIndex, material), 1.0);");
    }
}