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

    float32& distance = context.getPinData< typename Super::OutPin0 >();
    Vector3f32& gradient = context.getPinData< typename Super::OutPin1 >();

    CSGCube cube( center, halfExtents );
    Vector4f32 sample = cube.getValueAndGradient( worldPosition );
    gradient = Vector3f32( sample );
    distance = sample.W;
}

void SdfCubeVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                     rhi::ShaderGenerator& generator ) const {
    if( generator.getStage() != rhi::ShaderStage::Fragment )
        return;

    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addInclude( "includes/volume/csg/cube.h" );

    String cubeVariableName = format::format( "cubeNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "cubeSample_{:x}", getId().get() );
    String isoValueOutVariableName = format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgCube {} = CsgCube({}, {});\n",
        cubeVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode(
        format::format( "vec4 {} = GetValueAndGradient(worldPosition, {});\n", sampleVariableName, cubeVariableName ) );
    generator.appendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, sampleVariableName ) );
    generator.appendCode( format::format( "vec3 {} = {}.xyz;\n", gradientOutVariableName, sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfCubeVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Position";
    case InPin1::LocalId:
        return "Half Extents";
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
