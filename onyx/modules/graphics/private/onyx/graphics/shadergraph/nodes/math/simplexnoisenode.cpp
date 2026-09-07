#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void SimplexNoise2DNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SimplexNoise2DNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                           rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin& inputPin0 = getInputPin();

    generator.addImport( "includes.math.psrdnoise2d" );

    String noiseVariable = format::format( "noiseNode_{:x}", getId().get() );
    String noiseValueOutVariableName = format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() );
    String noiseGradientOutVariableName = format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() );

    generator.appendCode( format::format( "float2 {};\n", noiseGradientOutVariableName ) );
    generator.appendCode( format::format(
        "float {} = psrdnoise({}, float2(0.0f,0.0f), 0.0f, {});\n",
        noiseValueOutVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin >() ),
        noiseGradientOutVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SimplexNoise2DNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin::LocalId:
        return "Position";
    case OutPin0::LocalId:
        return "Noise Value";
    case OutPin1::LocalId:
        return "Noise Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif

void SimplexNoise3DNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SimplexNoise3DNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                           rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin& inputPin0 = getInputPin();

    generator.addImport( "includes.math.psrdnoise3d.h" );

    String noiseVariable = format::format( "noiseNode_{:x}", getId().get() );
    String noiseValueOutVariableName = format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() );
    String noiseGradientOutVariableName = format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() );

    generator.appendCode( format::format( "float3 {};\n", noiseGradientOutVariableName ) );
    generator.appendCode( format::format(
        "float {} = psrdnoise({}, float3(0.0f,0.0f, 0.0f), 0.0f, {});\n",
        noiseValueOutVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin >() ),
        noiseGradientOutVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SimplexNoise3DNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin::LocalId:
        return "Position";
    case OutPin0::LocalId:
        return "Noise Value";
    case OutPin1::LocalId:
        return "Noise Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif
} // namespace onyx::graphics::shader_graph_nodes
