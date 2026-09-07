#include <onyx/volume/shadergraph/nodes/sdfnoiseshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfNoise2DShaderGraphNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

node_graph::PinBase* SdfNoise2DShaderGraphNode::getInputPin( uint32_t index ) {
    if( index == 0 )
        return &m_noiseValueInPin;

    return &m_noiseGradientInPin;
}

const node_graph::PinBase* SdfNoise2DShaderGraphNode::getInputPin( uint32_t index ) const {
    if( index == 0 )
        return &m_noiseValueInPin;

    return &m_noiseGradientInPin;
}

node_graph::PinBase* SdfNoise2DShaderGraphNode::getOutputPin( uint32_t index ) {
    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

const node_graph::PinBase* SdfNoise2DShaderGraphNode::getOutputPin( uint32_t index ) const {
    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

void SdfNoise2DShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                  rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    if( ( context.isPinConnected< IsoValueOutPin >() == false ) &&
        ( context.isPinConnected< GradientOutPin >() == false ) )
        return;

    String isoValueOutVariableName = format::format( "pin_{:x}", m_isoValueOutPin.getGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", m_gradientOutPin.getGlobalId().get() );
    String gradientTmpVariableName = format::format( "noiseGradient_{:x}", getId().get() );

    generator.appendCode(
        format::format( "float {} = worldPosition.y - {};\n",
                        isoValueOutVariableName,
                        m_noiseValueInPin.isConnected()
                            ? format::format( "pin_{:x}", m_noiseValueInPin.getLinkedPinGlobalId().get() )
                            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< NoiseValueInPin >() ) ) );
    generator.appendCode( format::format(
        "float2 {} = {};\n",
        gradientTmpVariableName,
        m_noiseGradientInPin.isConnected()
            ? format::format( "pin_{:x}", m_noiseGradientInPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< NoiseGradientInPin >() ) ) );

    generator.appendCode( format::format( "float3 {0} = normalize( float3( {1}.x, 1.0, {1}.y ) );\n",
                                          gradientOutVariableName,
                                          gradientTmpVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfNoise2DShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case NoiseValueInPin::LocalId:
        return "Noise";
    case NoiseGradientInPin::LocalId:
        return "Noise Gradient";
    case IsoValueOutPin::LocalId:
        return "Iso Value";
    case GradientOutPin::LocalId:
        return "Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif

void SdfNoise3DShaderGraphNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

node_graph::PinBase* SdfNoise3DShaderGraphNode::getInputPin( uint32_t index ) {
    if( index == 0 )
        return &m_noiseValueInPin;

    return &m_noiseGradientInPin;
}

const node_graph::PinBase* SdfNoise3DShaderGraphNode::getInputPin( uint32_t index ) const {
    if( index == 0 )
        return &m_noiseValueInPin;

    return &m_noiseGradientInPin;
}

node_graph::PinBase* SdfNoise3DShaderGraphNode::getOutputPin( uint32_t index ) {
    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

const node_graph::PinBase* SdfNoise3DShaderGraphNode::getOutputPin( uint32_t index ) const {
    if( index == 0 )
        return &m_isoValueOutPin;

    return &m_gradientOutPin;
}

void SdfNoise3DShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                  rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment )
    //     return;

    if( ( context.isPinConnected< IsoValueOutPin >() == false ) &&
        ( context.isPinConnected< GradientOutPin >() == false ) )
        return;

    String isoValueOutVariableName = format::format( "pin_{:x}", m_isoValueOutPin.getGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", m_gradientOutPin.getGlobalId().get() );

    generator.appendCode(
        format::format( "float {} = worldPosition.y - {}.w;\n",
                        isoValueOutVariableName,
                        m_isoValueOutPin.isConnected()
                            ? format::format( "pin_{:x}", m_isoValueOutPin.getLinkedPinGlobalId().get() )
                            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< IsoValueOutPin >() ) ) );
    generator.appendCode( format::format(
        "float3 {} = normalize( {} );\n",
        gradientOutVariableName,
        m_noiseGradientInPin.isConnected()
            ? format::format( "pin_{:x}", m_noiseGradientInPin.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< NoiseGradientInPin >() ) ) );
}

#if ONYX_IS_EDITOR
StringView SdfNoise3DShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case NoiseValueInPin::LocalId:
        return "Noise";
    case NoiseGradientInPin::LocalId:
        return "Noise Gradient";
    case IsoValueOutPin::LocalId:
        return "Iso Value";
    case GradientOutPin::LocalId:
        return "Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif
} // namespace onyx::volume
