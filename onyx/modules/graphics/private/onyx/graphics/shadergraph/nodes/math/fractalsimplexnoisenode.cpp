#include <onyx/graphics/shadergraph/nodes/math/fractalsimplexnoisenode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

#include <onyx/math/psrdnoise2d.h>

namespace onyx::graphics::shader_graph_nodes {
void FractalSimplexNoise2DNode::onUpdate( node_graph::ExecutionContext& context ) const {
    const Vector2f32& inputPosition = context.getPinData< InPin >();

    float& outNoiseValue = context.getPinData< OutPin0 >();
    Vector2f32& outNoiseGradient = context.getPinData< OutPin1 >();

    Octave Octaves[] = {
        { 1e-04, 1000 },
        { 1e-06, 5000 },
    };

    Vector2f32 octaveNoiseGradient;
    for( uint32_t i = 0; i < 2; ++i ) {
        float octaveNoiseValue = psrdnoise( inputPosition * Octaves[ i ].Frequency, octaveNoiseGradient );

        outNoiseValue += octaveNoiseValue * Octaves[ i ].Amplitude;
        outNoiseGradient += octaveNoiseGradient * Octaves[ i ].Frequency * Octaves[ i ].Amplitude;
    }
}

void FractalSimplexNoise2DNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                  rhi::ShaderGenerator& generator ) const {
    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin& inputPin0 = getInputPin();

    generator.addImport( "includes.math.psrdnoise2d" );

    String noiseVariable{ format::format( "noiseNode_{:x}", getId().get() ) };
    String noiseValueOutVariableName{ format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() ) };
    String noiseGradientOutVariableName{ format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() ) };

    if( m_octaves.empty() ) {
        generator.appendCode( format::format( "float {} = 0.0f;\n", noiseValueOutVariableName ) );
        generator.appendCode( format::format( "float2 {} = float2( 0.0f, 0.0f );\n", noiseGradientOutVariableName ) );
        return;
    }
    // We could unroll the loop here, not sure if its worth it though

    generator.appendCode( "struct Octave { float Frequency; float Amplitude; };\n" );
    generator.appendCode( "Octave Octaves[] = { \n" );

    for( const Octave& octave : m_octaves ) {
        generator.appendCode( format::format( "{{ {}, {} }},\n", octave.Frequency, octave.Amplitude ) );
    }

    generator.appendCode( "};\n" );

    generator.appendCode( format::format( "float {} = 0.0f;\n", noiseValueOutVariableName ) );
    generator.appendCode( format::format( "float2 {} = float2( 0.0f, 0.0f );\n", noiseGradientOutVariableName ) );
    generator.appendCode( format::format( "for( uint32_t i = 0; i < {}; ++i ) {{ \n", m_octaves.size() ) );

    generator.appendCode( "float2 octaveGradient; \n" );
    generator.appendCode( format::format(
        "float octaveNoiseValue = psrdnoise( {} * Octaves[i].Frequency, \n",
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin >() ) ) );
    generator.appendCode( "float2( 0.0f, 0.0f ), \n" );
    generator.appendCode( "0.0f,\n" );
    generator.appendCode( "octaveGradient );\n" );

    generator.appendCode(
        format::format( "{} += octaveNoiseValue * Octaves[i].Amplitude;\n", noiseValueOutVariableName ) );
    generator.appendCode( format::format( "{} += octaveGradient * Octaves[i].Frequency * Octaves[i].Amplitude;\n",
                                          noiseGradientOutVariableName ) );

    generator.appendCode( "}\n" );
}

#if ONYX_IS_EDITOR
StringView FractalSimplexNoise2DNode::getPinName( StringId32 pinId ) const {
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
