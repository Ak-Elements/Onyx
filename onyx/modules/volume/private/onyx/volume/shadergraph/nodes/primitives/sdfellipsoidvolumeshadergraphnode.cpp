#include <onyx/volume/shadergraph/nodes/primitives/sdfellipsoidvolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfEllipsoidVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SdfEllipsoidVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                          rhi::ShaderGenerator& generator ) const {
    if( generator.getStage() != rhi::ShaderStage::Fragment )
        return;

    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addInclude( "includes/volume/csg/ellipsoid.h" );

    String ellipsoidVariableName = format::format( "ellipsoidNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "ellipsoidSample_{:x}", getId().get() );
    String isoValueOutVariableName = format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgEllipsoid {} = CsgEllipsoid({}, {});\n",
        ellipsoidVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode( format::format( "vec4 {} = GetValueAndGradient(worldPosition, {});\n",
                                          sampleVariableName,
                                          ellipsoidVariableName ) );
    generator.appendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, sampleVariableName ) );
    generator.appendCode( format::format( "vec3 {} = {}.xyz;\n", gradientOutVariableName, sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfEllipsoidVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Position";
    case InPin1::LocalId:
        return "Radii";
    case OutPin0::LocalId:
        return "Iso Value";
    case OutPin1::LocalId:
        return "Gradient";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
