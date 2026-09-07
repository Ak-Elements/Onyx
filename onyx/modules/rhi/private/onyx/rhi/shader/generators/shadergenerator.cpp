#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

#include <utility>

namespace onyx::rhi {
namespace {
#if ONYX_ASSERT_ENABLED
constexpr uint8_t MaxTextures = 8;
#endif

String generateShaderVariable( const ShaderVariable& variable ) {
    StringView typeAsString = rhi::ShaderGenerator::shaderTypeToString( variable.Type );

    switch( variable.Semantic ) {
    case ShaderSemantic::None:
        return format::format( "{} {}", typeAsString, variable.Name.getString() );
    case ShaderSemantic::Position:
        return format::format( "{} {} : SV_POSITION", typeAsString, variable.Name.getString() );
    case ShaderSemantic::Target:
        return format::format( "{} {} : SV_Target{}", typeAsString, variable.Name.getString(), variable.SemanticIndex );
    case ShaderSemantic::Depth:
        return format::format( "{} {} : SV_DEPTH", typeAsString, variable.Name.getString() );
    case ShaderSemantic::VertexId:
        return format::format( "{} {} : SV_VertexID", typeAsString, variable.Name.getString() );
    case ShaderSemantic::InstanceId:
        return format::format( "{} {} : SV_InstanceID", typeAsString, variable.Name.getString() );
    case ShaderSemantic::DispatchThreadId:
        return format::format( "uint3 {} : SV_DispatchThreadID", variable.Name.getString() );
    case ShaderSemantic::GroupId:
        return format::format( "uint3 {} : SV_DispatchGroupID", variable.Name.getString() );
    case ShaderSemantic::GroupThreadId:
        return format::format( "uint3 {} : SV_GroupThreadID", variable.Name.getString() );
    case ShaderSemantic::GroupIndex:
        return format::format( "uint {} : SV_GroupIndex", variable.Name.getString() );
    }

    return "";
}

String getPushConstantTypeString( const ShaderPushConstant& constant ) {
    if( constant.Type == ShaderDataType::Pointer ) {
        return format::format( "Ptr<{}>", constant.SubType );
    }

    return String( rhi::ShaderGenerator::shaderTypeToString( constant.Type ) );
}
template < typename T > requires std::is_invocable_r_v< bool, T, const ShaderVariable& >
String generateStageInput( ShaderStage stage,
                           const DynamicArray< ShaderVariable >& inputs,
                           String& outCode,
                           T filterFunction ) {
    DynamicArray< String > inStructMembers;
    DynamicArray< String > entryPointInputParameters;
    for( const ShaderVariable& input : inputs ) {
        if( filterFunction( input ) ) {
            inStructMembers.push_back( generateShaderVariable( input ) );
        } else {
            entryPointInputParameters.push_back( generateShaderVariable( input ) );
        }
    }

    if( inStructMembers.empty() == false ) {
        const uint32_t memberCount = inStructMembers.size();
        if( memberCount > 1 ) {
            String inputTypeName = format::format( "{}Input\n", stage );
            outCode += format::format( "struct {} {{\n", inputTypeName );
            outCode += join( inStructMembers, ";\n" ) + ";\n"; // add last ; and newline
            outCode += "};\n";
            entryPointInputParameters.insert( entryPointInputParameters.begin(),
                                              format::format( "{} input", inputTypeName ) );
        } else {
            entryPointInputParameters.insert( entryPointInputParameters.begin(), inStructMembers[ 0 ] );
        }
    }

    return join( entryPointInputParameters, ", " );
}

void generateStageOutput( ShaderStage stage, const DynamicArray< ShaderVariable >& outputs, String& outCode ) {
    outCode += format::format( "struct {}Output {{\n", stage );
    for( const ShaderVariable& output : outputs ) {
        outCode += generateShaderVariable( output ) + ";\n";
    }
    outCode += "};\n";
}

// vertex / fragment
template < typename T > requires std::is_invocable_r_v< bool, T, const ShaderVariable& >
void generateGraphicsStage( const ShaderGenerator::StageGenerationContext& stageContext,
                            String& outCode,
                            T inputFilterFunction ) {
    ONYX_ASSERT( stageContext.Stage == ShaderStage::Vertex || stageContext.Stage == ShaderStage::Fragment );
    ONYX_ASSERT( stageContext.Outputs.empty() == false, "{} stage requires an Outputs struct.", stageContext.Stage );

    String entryPointParameters = generateStageInput( stageContext.Stage,
                                                      stageContext.Inputs,
                                                      outCode,
                                                      inputFilterFunction );
    String outputType;
    const uint32_t outputsCount = numericCast< uint32_t >( stageContext.Outputs.size() );
    if( outputsCount == 1 ) {
        outputType = rhi::ShaderGenerator::shaderTypeToString( stageContext.Outputs[ 0 ].Type );
    } else {
        generateStageOutput( stageContext.Stage, stageContext.Outputs, outCode );
        outputType = format::format( "{}Output", stageContext.Stage );
    }

    String lowercaseStageString = toLower( format::format( "{}", stageContext.Stage ) );
    String entryPointPrefix = lowercaseStageString;

    outCode += format::format( "[shader(\"{}\")]\n", lowercaseStageString );
    outCode += format::format( "{} {}Main({})\n{{\n", outputType, entryPointPrefix, entryPointParameters );
    outCode += format::format( "{} output;\n", outputType );
    outCode += stageContext.Code;
    outCode += "\nreturn output;\n";
    outCode += "}\n\n";
}
} // namespace

ShaderGenerator::ShaderGenerator( ShaderStage stage )
    : m_currentStage( stage ) {}

void ShaderGenerator::appendCode( StringView code ) {
    StageGenerationContext& stage = getOrCreateStage( m_currentStage );
    stage.Code.append( code );
}

bool ShaderGenerator::hasPushConstant( StringId32 name ) const {
    return std::ranges::any_of( m_pushConstants,
                                [ & ]( const ShaderPushConstant& variable ) { return variable.Name == name; } );
}

void ShaderGenerator::addPushConstant( ShaderPushConstant constant ) {
    ONYX_ASSERT( hasPushConstant( constant.Name ) == false, "Push constant with that name already exists." );
    m_pushConstants.push_back( std::move( constant ) );
}

void ShaderGenerator::addInput( ShaderVariable input ) {
    addInput( m_currentStage, std::move( input ) );
}

void ShaderGenerator::addInput( ShaderStage stage, ShaderVariable input ) {
    StageGenerationContext& context = getOrCreateStage( stage );
#if !ONYX_IS_RETAIL
    // ensure semantic is matching with the type
    switch( input.Semantic ) {
    case ShaderSemantic::Position:
    case ShaderSemantic::Target: {
        if( input.Type != ShaderDataType::Float4 ) {
            ONYX_LOG_ERROR( "{} has to be of type float4", input.Semantic );
            // return error
        }
        break;
    }
    case ShaderSemantic::Depth: {
        ONYX_LOG_ERROR( "Depth is not allowed as input" );
        break;
    }
    case ShaderSemantic::DispatchThreadId:
    case ShaderSemantic::GroupId:
    case ShaderSemantic::GroupThreadId: {
        if( input.Type != ShaderDataType::UInt3 ) {
            ONYX_LOG_ERROR( "{} has to be of type uint3", input.Semantic );
            // return error
        }
        break;
    }
    case ShaderSemantic::VertexId:
    case ShaderSemantic::InstanceId:
    case ShaderSemantic::GroupIndex: {
        if( input.Type != ShaderDataType::UInt ) {
            ONYX_LOG_ERROR( "{} has to be of type uint", input.Semantic );
            // return error
        }
        break;
    }
    case ShaderSemantic::None:
        break;
    }
#endif
    context.Inputs.push_back( std::move( input ) );
}

void ShaderGenerator::addOutput( ShaderVariable input ) {
    addOutput( m_currentStage, input );
}

void ShaderGenerator::addOutput( ShaderStage stage, ShaderVariable output ) {
    StageGenerationContext& context = getOrCreateStage( stage );
#if !ONYX_IS_RETAIL
    // ensure semantic is matching with the type
    switch( output.Semantic ) {
    case ShaderSemantic::Position:
    case ShaderSemantic::Target: {
        if( output.Type != ShaderDataType::Float4 ) {
            ONYX_LOG_ERROR( "{} has to be of type float4", output.Semantic );
            output.Type = ShaderDataType::Float4;
            // return error
        }
        break;
    }
    case ShaderSemantic::Depth: {
        if( output.Type != ShaderDataType::Float ) {
            ONYX_LOG_ERROR( "Depth has to be of type float" );
            output.Type = ShaderDataType::Float;
            // return error
        }
        break;
    }
    case ShaderSemantic::DispatchThreadId:
    case ShaderSemantic::GroupId:
    case ShaderSemantic::GroupThreadId:
    case ShaderSemantic::VertexId:
    case ShaderSemantic::InstanceId:
    case ShaderSemantic::GroupIndex: {
        ONYX_LOG_ERROR( "{} is not allowed as an output.", output.Semantic );
        break;
    }
    case ShaderSemantic::None:
        break;
    }
#endif
    context.Outputs.push_back( std::move( output ) );
}

void ShaderGenerator::addImport( String include ) {
    m_shaderImports.emplace( include );
}

void ShaderGenerator::addInclude( String include ) {
    m_shaderIncludes.emplace( include );
}

void ShaderGenerator::generatePushConstants( String& stageCode ) {
    // TODO: Probably need to add padding

    if( m_pushConstants.empty() && m_textures.empty() ) {
        return;
    }

    stageCode += "struct PushConstants {\n";

    for( const ShaderPushConstant& variable : m_pushConstants ) {
        String pushConstantType = getPushConstantTypeString( variable );
        stageCode += format::format( "{} {};\n", pushConstantType, variable.Name.getString() );
    }

    // Check if enough space for texture indices
    if( m_textures.empty() == false ) {
        ONYX_ASSERT( static_cast< uint8_t >( m_textures.size() ) <= MaxTextures );
        stageCode += "uint32_t TextureIndices[8]; \n";
    }

    stageCode += "};\n";
    stageCode += "[ push_constant ]\n";
    stageCode += "PushConstants Constants;\n";
}

void ShaderGenerator::generateIncludes( String& stageCode ) {
    for( const String& include : m_shaderIncludes ) {
        stageCode += format::format( "#include \"{}\"\n", include );
    }

    stageCode += "\n";
}

void ShaderGenerator::generateImports( String& stageCode ) {
    for( const String& include : m_shaderImports ) {
        stageCode += format::format( "import {};\n", include );
    }

    stageCode += "\n";
}

String ShaderGenerator::generateShaderStageCode( const ShaderGenerator::StageGenerationContext& stageContext ) const {
    String code;

    switch( stageContext.Stage ) {
    case ShaderStage::Vertex: {
        generateGraphicsStage( stageContext, code, []( const ShaderVariable& input ) {
            return input.Semantic == ShaderSemantic::None;
        } );
        break;
    }
    case ShaderStage::Fragment:
        generateGraphicsStage( stageContext, code, []( const ShaderVariable& input ) {
            return ( input.Semantic == ShaderSemantic::None ) || ( input.Semantic == ShaderSemantic::Position );
        } );
        break;
    case ShaderStage::Compute:
    case ShaderStage::Hull:
    case ShaderStage::Domain:
    case ShaderStage::Geometry:
    case ShaderStage::RayGeneration:
    case ShaderStage::Intersection:
    case ShaderStage::AnyHit:
    case ShaderStage::ClosestHit:
    case ShaderStage::Miss:
    case ShaderStage::Callable:
    case ShaderStage::Mesh:
    case ShaderStage::Amplification:
    case ShaderStage::Dispatch:
    case ShaderStage::Invalid:
    case ShaderStage::All:
        break;
    }

    return code;
}

String ShaderGenerator::generateShader() {
    using UnderlyingType = std::underlying_type< ShaderStage >::type;
    constexpr UnderlyingType Count = enums::toIntegral( ShaderStage::Count );
    for( UnderlyingType i = 0; i < Count; ++i ) {
        const ShaderStage stage = enums::toEnum< ShaderStage >( 1 << i );
        if( stage == ShaderStage::Invalid || stage == ShaderStage::All ) {
            continue;
        }

        m_currentStage = stage;

        Optional< StageGenerationContext* > stageContextOptional = getStage( stage );
        if( stageContextOptional.has_value() ) {
            generateShaderStage( *stageContextOptional.value() );
        } else {
            StageGenerationContext context{ stage };
            generateShaderStage( context );
            if( context.Code.empty() == false ) {
                m_shaderStages.push_back( std::move( context ) );
            }
        }
    }

    Optional< ShaderGenerator::StageGenerationContext* > vertexStageOptional = getStage( ShaderStage::Vertex );
    if( StageGenerationContext* vertexStage = vertexStageOptional.value_or( nullptr ) ) {
        auto it = std::ranges::find_if( vertexStage->Outputs, []( const ShaderVariable& input ) {
            return input.Semantic == ShaderSemantic::Position;
        } );

        if( it == vertexStage->Outputs.end() ) {
            vertexStage->Outputs.emplace( vertexStage->Outputs.begin(),
                                          "Position",
                                          ShaderDataType::Float4,
                                          ShaderSemantic::Position );
        }
    }

    Optional< ShaderGenerator::StageGenerationContext* > fragmentStageOptional = getStage( ShaderStage::Fragment );
    if( StageGenerationContext* fragmentStage = fragmentStageOptional.value_or( nullptr ) ) {
        if( fragmentStage->Inputs.empty() ) {
            fragmentStage->Inputs = vertexStageOptional.value()->Outputs;
        }

        const bool hasOutputTarget = std::ranges::any_of( fragmentStage->Outputs, []( const ShaderVariable& input ) {
            return input.Semantic == ShaderSemantic::Target;
        } );

        if( hasOutputTarget == false ) {
            ONYX_LOG_ERROR( "Shader is missing output target" );
        }
    }

    String code;
    generateImports( code );
    generateIncludes( code );

    generatePushConstants( code );

    Optional< ShaderGenerator::StageGenerationContext* > globalStageOptional = getStage( ShaderStage::All );
    if( StageGenerationContext* globalStage = globalStageOptional.value_or( nullptr ) ) {
        generateShaderStage( *globalStage );
        code += globalStage->Code;
        code += "\n";
    } else {
        StageGenerationContext context{ ShaderStage::All };
        generateShaderStage( context );
        if( context.Code.empty() == false ) {
            code += globalStage->Code;
            code += "\n";
        }
    }

    String shaderStagesCode;
    for( const StageGenerationContext& stage : m_shaderStages ) {
        code += generateShaderStageCode( stage );
    }

    return code;
}

ShaderGenerator::StageGenerationContext& ShaderGenerator::getOrCreateStage( ShaderStage stage ) {
    auto it = std::ranges::find_if( m_shaderStages, [ stage ]( StageGenerationContext& stageData ) {
        return stage == stageData.Stage;
    } );

    if( it == m_shaderStages.end() ) {
        return m_shaderStages.emplace_back( stage );
    }

    return *it;
}

// TODO: Change to & in c++26
Optional< ShaderGenerator::StageGenerationContext* > ShaderGenerator::getStage( ShaderStage stage ) {
    auto it = std::ranges::find_if( m_shaderStages, [ stage ]( StageGenerationContext& stageData ) {
        return stage == stageData.Stage;
    } );

    if( it == m_shaderStages.end() ) {
        return std::nullopt;
    }

    return &( *it );
}

PBRShaderGenerator::PBRShaderGenerator() {
    // addImport( "includes/common.h" );
    addImport( "includes.viewconstants" );
    addImport( "includes.math.matrix" );

    addImport( "lighting" );

    addPushConstant( { "Model", ShaderDataType::Mat4 } );
    addPushConstant( { "ViewConstants", ShaderDataType::Pointer, "ViewConstants" } );
    addPushConstant( { "LightEnvironment", ShaderDataType::Pointer, "LightEnvironment" } );
}

void PBRShaderGenerator::generateShaderStage( StageGenerationContext& stageContext ) {
    if( stageContext.Stage == ShaderStage::Vertex ) {
        generateVertexStage();
    } else if( stageContext.Stage == ShaderStage::Fragment ) {
        generateFragmentStage();
    }
}

void PBRShaderGenerator::generateVertexStage() {
    String vertexShaderCode;

    addInput( { "Position", ShaderDataType::Float3 } );
    addInput( { "Normal", ShaderDataType::Float3 } );
    addInput( { "Uv", ShaderDataType::Float2 } );

    addOutput( { "Position", ShaderDataType::Float4, ShaderSemantic::Position } );
    addOutput( { "WorldPosition", ShaderDataType::Float3 } );
    addOutput( { "WorldNormal", ShaderDataType::Float3 } );

    StringView body = "output.WorldPosition = input.Position;\n"
                      "output.WorldNormal = input.Normal;\n"
                      "output.Position = mul( Constants.ViewConstants->ProjectionMatrix, "
                      "Constants.ViewConstants->ViewMatrix ) * "
                      "float4(input.Position, 1.0);";

    appendCode( body );
}

void PBRShaderGenerator::generateFragmentStage() {
    addOutput( { "Color", ShaderDataType::Float4, ShaderSemantic::Target } );

    appendCode( "float3 worldPosition = input.WorldPosition;\n" );
    appendCode( "float3 worldNormal = input.WorldNormal;\n" );
    // appendCode( "uint32_t clusterIndex = getClusterIndex(gl_FragCoord, LightClusterGridSize, LightClusterSize, "
    //             "LightClusterScale, LightClusterBias);\n" );
    appendCode(
        "return float4(calculatePBRLighting(worldPosition, worldNormal, Constants.ViewConstants->CameraPosition, "
        "input.Position, LightClusterScale, LightClusterBias, clusterIndex, material, DebugFlag), 1.0);" );
}

} // namespace onyx::rhi
