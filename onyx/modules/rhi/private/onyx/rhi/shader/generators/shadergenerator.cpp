#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::rhi {
namespace {
#if ONYX_ASSERT_ENABLED
constexpr uint8_t MaxTextures = 8;
#endif
} // namespace

void ShaderGenerator::generateVertexShader() {
    setStage( ShaderStage::Vertex );

    String vertexShaderCode;

    generateIncludes( vertexShaderCode );

    m_vertexInputs.emplace_back( "InPosition", ShaderDataType::Float3 );
    m_vertexInputs.emplace_back( "InUVX", ShaderDataType::Float );
    m_vertexInputs.emplace_back( "InNormal", ShaderDataType::Float3 );
    m_vertexInputs.emplace_back( "InUVY", ShaderDataType::Float );

    m_vertexOutputs.emplace_back( "WorldPosition", ShaderDataType::Float3 );
    m_vertexOutputs.emplace_back( "WorldNormal", ShaderDataType::Float3 );

    uint32_t locationIndex = 0;
    for( const ShaderVariable& vertexInput : m_vertexInputs ) {
        vertexShaderCode += format::format( "layout (location = {}) in {} {};\n",
                                            locationIndex++,
                                            vertexInput.Type,
                                            vertexInput.Name );
    }

    if( m_vertexOutputs.empty() == false ) {
        vertexShaderCode += "struct OutStruct\n{\n";

        for( const ShaderVariable& vertexOutput : m_vertexOutputs ) {
            vertexShaderCode += format::format( "{} {};\n", vertexOutput.Type, vertexOutput.Name );
        }

        vertexShaderCode += "};\n";
        vertexShaderCode += "layout(location = 0) out OutStruct Output;\n";
    }

    generatePushConstants( vertexShaderCode );

    StringView
        body = "Output.WorldPosition = InPosition;\n"
               "Output.WorldNormal = InNormal;\n"
               "gl_Position = u_ViewConstants.ProjectionMatrix * u_ViewConstants.ViewMatrix * vec4(InPosition, 1.0);";

    vertexShaderCode += format::format( "void main() \n{{ \n {} \n}}\n", body );

    // TODO: add custom vertex code if there was one
    // vertexShaderCode += format::Format("{}\n", shaderStagesCode[enums::ToIntegral(ShaderStage::Vertex)]);

    m_shaderStagesCode[ enums::toIntegral( ShaderStage::Vertex ) ] = vertexShaderCode;
}

void ShaderGenerator::appendCode( StringView code ) {
    String& shaderCode = m_shaderStagesCode[ enums::toIntegral( m_currentStage ) ];
    shaderCode.append( code );
}

bool ShaderGenerator::hasPushConstant( StringView name ) const {
    return hasPushConstant( m_currentStage, name );
}

bool ShaderGenerator::hasPushConstant( ShaderStage stage, StringView name ) const {
    return std::ranges::any_of( m_pushConstants[ enums::toIntegral( stage ) ],
                                [ & ]( const ShaderVariable& variable ) { return variable.Name == name; } );
}

void ShaderGenerator::addPushConstant( StringView name, ShaderDataType type ) {
    addPushConstant( m_currentStage, name, type, 0 );
}

void ShaderGenerator::addPushConstant( ShaderStage stage, StringView name, ShaderDataType type ) {
    addPushConstant( stage, name, type, 0 );
}

void ShaderGenerator::addPushConstant( ShaderStage stage, StringView name, ShaderDataType type, uint32_t offset ) {
    DynamicArray< ShaderVariable >& stagePushConstants = m_pushConstants[ enums::toIntegral( stage ) ];
    ONYX_ASSERT( hasPushConstant( stage, name ) == false, "Push constant with that name already exists." );

    stagePushConstants.emplace_back( String( name ), type, offset );
}

void ShaderGenerator::addInclude( String include ) {
    m_shaderIncludes.emplace( include );
}

void ShaderGenerator::generatePushConstants( String& stageCode ) {
    // TODO: Probably need to add padding

    const DynamicArray< ShaderVariable >& stagePushConstants = m_pushConstants[ enums::toIntegral( m_currentStage ) ];
    if( ( ( m_currentStage == ShaderStage::Vertex ) && stagePushConstants.empty() ) ||
        ( ( m_currentStage == ShaderStage::Fragment ) && stagePushConstants.empty() && m_textures.empty() ) ) {
        return;
    }

    stageCode += "layout( push_constant ) uniform PushConstants \n";
    stageCode += "{ \n";

    for( const ShaderVariable& variable : stagePushConstants ) {
        if( variable.Offset == 0 ) {
            stageCode += format::format( "{} {};\n", variable.Type, variable.Name );
        } else {
            stageCode += format::format( "layout(offset = {}) {} {};\n",
                                         variable.Offset,
                                         variable.Type,
                                         variable.Name );
        }
    }

    // Check if enough space for texture indices
    if( ( m_currentStage == ShaderStage::Fragment ) && ( m_textures.empty() == false ) ) {
        ONYX_ASSERT( static_cast< uint8_t >( m_textures.size() ) <= MaxTextures );
        stageCode += "uint TextureIndices[8]; \n";
    }

    stageCode += "}; \n";
}

void ShaderGenerator::generateIncludes( String& stageCode ) {
    for( const String& include : m_shaderIncludes ) {
        stageCode += format::format( "#include \"{}\"\n", include );
    }

    stageCode += "\n";
}

void ShaderGenerator::generateFragmentShader() {
    setStage( ShaderStage::Fragment );

    String fragmentShaderCode;

    generateIncludes( fragmentShaderCode );

    if( m_vertexOutputs.empty() == false ) {
        fragmentShaderCode += "layout(location = 0) in InStruct \n{\n";

        for( const ShaderVariable& vertexOutput : m_vertexOutputs ) {
            String typeAsString = vertexOutput.Type == ShaderDataType::Float3 ? "vec3" : "vec2";
            fragmentShaderCode += format::format( "{} {};\n", typeAsString, vertexOutput.Name );
        }

        fragmentShaderCode += "};\n";
    }

    fragmentShaderCode += "layout(location = 0) out vec4 outColor;\n";

    generatePushConstants( fragmentShaderCode );

    doGenerateFragmentMain();
    fragmentShaderCode += format::format( "void main() \n{{ \n {} \n}}\n",
                                          m_shaderStagesCode[ enums::toIntegral( ShaderStage::Fragment ) ] );

    m_shaderStagesCode[ enums::toIntegral( ShaderStage::Fragment ) ] = fragmentShaderCode;
}

String ShaderGenerator::generateShader() {
    generateVertexShader();
    generateFragmentShader();

    String commonIncludes;
    for( const String& include : m_shaderIncludes ) {
        commonIncludes += format::format( "#include \"{}\"\n", include );
    }

    return format::format( "#version 460 core\n{}\nvertex\n{{\n{}\n}} \nfragment\n{{\n{}\n}}",
                           commonIncludes,
                           m_shaderStagesCode[ enums::toIntegral( ShaderStage::Vertex ) ],
                           m_shaderStagesCode[ enums::toIntegral( ShaderStage::Fragment ) ] );
}

PBRShaderGenerator::PBRShaderGenerator() {
    addInclude( "includes/common.h" );
    addInclude( "includes/viewconstants.h" );

    addInclude( "includes/lighting.h" );

    addPushConstant( ShaderStage::Vertex, "Model", ShaderDataType::Mat4 );

    addPushConstant( ShaderStage::Fragment, "LightClusterGridSize", ShaderDataType::UInt3, 64 );
    addPushConstant( ShaderStage::Fragment, "LightClusterScale", ShaderDataType::Float );
    addPushConstant( ShaderStage::Fragment, "LightClusterSize", ShaderDataType::UInt2 );
    addPushConstant( ShaderStage::Fragment, "LightClusterBias", ShaderDataType::Float );
    addPushConstant( ShaderStage::Fragment, "DebugFlag", ShaderDataType::UInt );
}

void PBRShaderGenerator::doGenerateFragmentMain() {
    appendCode( "vec3 worldPosition = WorldPosition;\n" );
    appendCode( "vec3 worldNormal = WorldNormal;\n" );
    appendCode( "uint clusterIndex = GetClusterIndex(gl_FragCoord, LightClusterGridSize, LightClusterSize, "
                "LightClusterScale, LightClusterBias);\n" );
    appendCode( "outColor = vec4(CalculatePBRLighting(worldPosition, worldNormal, u_ViewConstants.CameraPosition, "
                "gl_FragCoord, LightClusterScale, LightClusterBias, clusterIndex, material, DebugFlag), 1.0);" );
}
} // namespace onyx::rhi
