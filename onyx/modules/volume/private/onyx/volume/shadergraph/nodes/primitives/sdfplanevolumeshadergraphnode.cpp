#include <onyx/volume/shadergraph/nodes/primitives/sdfplanevolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/volume/source/csg/csgplane.h>

namespace onyx::volume {
void SdfPlaneVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const Vector3f32& planeNormal = context.getPinData< typename Super::InPin0 >();
    const float32& planeOffset = context.getPinData< typename Super::InPin1 >();

    SdfSample& outSample = context.getPinData< typename Super::OutPin >();

    CSGPlane plane( planeOffset, planeNormal );
    Vector4f32 sample = plane.getValueAndGradient( worldPosition );
    outSample.Gradient = Vector3f32( sample );
    outSample.Distance = sample.W;
}

void SdfPlaneVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                      rhi::ShaderGenerator& generator ) const {
    if( context.isPinConnected< OutPin >() == false )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addImport( "includes.volume.csg.plane" );

    String planeVariableName = format::format( "planeNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "planeSample_{:x}", getId().get() );
    String outSdfSampleVariableName = format::format( "pin_{:x}", getOutputPin().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgPlane {} = CsgPlane( {}, {} );\n",
        planeVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode( format::format( "float4 {} = {}.getValueAndGradient( worldPosition );\n",
                                          sampleVariableName,
                                          planeVariableName ) );

    generator.appendCode( format::format( "SdfSample {0} =  SdfSample( {1}.xyz, {1}.w );\n",
                                          outSdfSampleVariableName,
                                          sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfPlaneVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Normal";
    case InPin1::LocalId:
        return "Distance";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
