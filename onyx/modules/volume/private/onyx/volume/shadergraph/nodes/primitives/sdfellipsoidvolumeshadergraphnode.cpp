#include <onyx/volume/shadergraph/nodes/primitives/sdfellipsoidvolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/volume/source/csg/csgsphere.h>

namespace onyx::volume {
void SdfEllipsoidVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    // TODO: Implement ellisoid
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const Vector3f32& center = context.getPinData< typename Super::InPin0 >();
    const Vector3f32 radii = context.getPinData< typename Super::InPin1 >();

    SdfSample& sdfSample = context.getPinData< typename Super::OutPin >();

    CSGSphere sphere( radii.Y, center );
    Vector4f32 sample = sphere.getValueAndGradient( worldPosition );
    sdfSample.Gradient = Vector3f32( sample );
    sdfSample.Distance = sample.W;
}

void SdfEllipsoidVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                          rhi::ShaderGenerator& generator ) const {
    if( ( context.isPinConnected< OutPin >() == false ) )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addImport( "includes.volume.csg.ellipsoid" );

    String ellipsoidVariableName = format::format( "ellipsoidNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "ellipsoidSample_{:x}", getId().get() );
    String sampleOutVariableName = format::format( "pin_{:x}", getOutputPin().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgEllipsoid {} = CsgEllipsoid( {}, {} );\n",
        ellipsoidVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode( format::format( "float4 {} = {}.getValueAndGradient( worldPosition );\n",
                                          sampleVariableName,
                                          ellipsoidVariableName ) );

    generator.appendCode( format::format( "SdfSample {0} =  SdfSample( {1}.xyz, {1}.w );\n",
                                          sampleOutVariableName,
                                          sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfEllipsoidVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Position";
    case InPin1::LocalId:
        return "Radii";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
