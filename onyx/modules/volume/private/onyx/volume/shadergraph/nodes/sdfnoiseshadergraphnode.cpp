#include <onyx/volume/shadergraph/nodes/sdfnoiseshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfNoise2DShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const float32 noiseValue = context.getPinData< InPin0 >();
    const Vector2f32 noiseGradient = context.getPinData< InPin1 >();

    SdfSample& sdfSample = context.getPinData< OutPin >();

    sdfSample.Gradient = Vector3f32( -noiseGradient.X, 1.0f, -noiseGradient.Y ).normalized();
    sdfSample.Distance = worldPosition.Y - noiseValue;
}

void SdfNoise2DShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                  rhi::ShaderGenerator& generator ) const {
    if( ( context.isPinConnected< OutPin >() == false ) )
        return;

    const InPin0& noiseValueInPin = getInputPin0();
    const InPin1& noiseGradientInPin = getInputPin1();

    String sampleOutVariableName{ format::format( "pin_{:x}", getOutputPin().getGlobalId().get() ) };

    String isoValueVariableName{ format::format( "isoValue_{:x}", getId().get() ) };
    String gradientValueVariableName{ format::format( "gradient_{:x}", getId().get() ) };

    generator.appendCode( format::format(
        "float {} = worldPosition.y - {};\n",
        isoValueVariableName,
        noiseValueInPin.isConnected() ? format::format( "pin_{:x}", noiseValueInPin.getLinkedPinGlobalId().get() )
                                      : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPin0 >() ) ) );

    generator.appendCode(
        format::format( "float3 {0} = normalize( float3( -{1}.x, 1.0f, -{1}.y ) );\n",
                        gradientValueVariableName,
                        noiseGradientInPin.isConnected()
                            ? format::format( "pin_{:x}", noiseGradientInPin.getLinkedPinGlobalId().get() )
                            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPin1 >() ) ) );

    generator.appendCode( format::format( "SdfSample {} =  SdfSample( {}.xyz, {} );\n",
                                          sampleOutVariableName,
                                          gradientValueVariableName,
                                          isoValueVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfNoise2DShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Noise";
    case InPin1::LocalId:
        return "Noise Gradient";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif

void SdfNoise3DShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const float32 noiseValue = context.getPinData< InPin0 >();
    const Vector3f32 noiseGradient = context.getPinData< InPin1 >();

    SdfSample& sdfSample = context.getPinData< OutPin >();

    sdfSample.Gradient = noiseGradient.normalized();
    sdfSample.Distance = worldPosition.Y - noiseValue;
}

void SdfNoise3DShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                  rhi::ShaderGenerator& generator ) const {
    if( ( context.isPinConnected< OutPin >() == false ) )
        return;

    const InPin0& noiseValueInPin = getInputPin0();
    const InPin1& noiseGradientInPin = getInputPin1();

    String sampleOutVariableName{ format::format( "pin_{:x}", getOutputPin().getGlobalId().get() ) };

    String isoValueVariableName{ format::format( "isoValue_{:x}", getId().get() ) };
    String gradientValueVariableName{ format::format( "gradient_{:x}", getId().get() ) };

    generator.appendCode( format::format(
        "float {} = worldPosition.y - {};\n",
        isoValueVariableName,
        noiseValueInPin.isConnected() ? format::format( "pin_{:x}", noiseValueInPin.getLinkedPinGlobalId().get() )
                                      : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPin0 >() ) ) );

    generator.appendCode(
        format::format( "float3 {} = normalize( {} );\n",
                        gradientValueVariableName,
                        noiseGradientInPin.isConnected()
                            ? format::format( "pin_{:x}", noiseGradientInPin.getLinkedPinGlobalId().get() )
                            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< InPin1 >() ) ) );

    generator.appendCode( format::format( "SdfSample {} =  SdfSample( {}.xyz, {} );\n",
                                          sampleOutVariableName,
                                          gradientValueVariableName,
                                          isoValueVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfNoise3DShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Noise";
    case InPin1::LocalId:
        return "Noise Gradient";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}
#endif
} // namespace onyx::volume
