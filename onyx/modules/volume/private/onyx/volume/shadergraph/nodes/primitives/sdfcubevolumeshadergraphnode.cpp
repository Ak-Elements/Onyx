#include <onyx/volume/shadergraph/nodes/primitives/sdfcubevolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

#include <onyx/volume/source/csg/csgcube.h>

namespace onyx::volume {
void SdfCubeVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const Vector3f32& center = context.getPinData< typename Super::InPin0 >();
    const Vector3f32& halfExtents = context.getPinData< typename Super::InPin1 >();

    SdfSample& sdfSample = context.getPinData< typename Super::OutPin >();

    CSGCube cube( center, halfExtents );
    Vector4f32 sample = cube.getValueAndGradient( worldPosition );
    sdfSample.Gradient = Vector3f32( sample );
    sdfSample.Distance = sample.W;
}

void SdfCubeVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                     rhi::ShaderGenerator& generator ) const {
    if( ( context.isPinConnected< OutPin >() == false ) )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addImport( "includes.volume.csg.cube" );

    String cubeVariableName = format::format( "cubeNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "cubeSample_{:x}", getId().get() );
    String sampleOutVariableName = format::format( "pin_{:x}", getOutputPin().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgCube {} = CsgCube( {}, {} );\n",
        cubeVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode( format::format( "float4 {} = {}.getValueAndGradient( worldPosition );\n",
                                          sampleVariableName,
                                          cubeVariableName ) );

    generator.appendCode( format::format( "SdfSample {0} =  SdfSample( {1}.xyz, {1}.w );\n",
                                          sampleOutVariableName,
                                          sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfCubeVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Position";
    case InPin1::LocalId:
        return "Half Extents";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
