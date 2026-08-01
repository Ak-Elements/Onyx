#include <onyx/volume/shadergraph/nodes/primitives/sdfspherevolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/volume/source/csg/csgsphere.h>

namespace onyx::volume {
void SdfSphereVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    Vector3f32 worldPosition = context.get< Vector3f32 >();
    const Vector3f32& center = context.getPinData< typename Super::InPin0 >();
    const float32 radius = context.getPinData< typename Super::InPin1 >();

    float32& distance = context.getPinData< typename Super::OutPin0 >();
    Vector3f32& gradient = context.getPinData< typename Super::OutPin1 >();

    CSGSphere sphere( radius, center );
    Vector4f32 sample = sphere.getValueAndGradient( worldPosition );
    gradient = Vector3f32( sample );
    distance = sample.W;
}

void SdfSphereVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                       rhi::ShaderGenerator& generator ) const {
    if( generator.getStage() != rhi::ShaderStage::Fragment )
        return;

    if( ( context.isPinConnected< OutPin0 >() == false ) && ( context.isPinConnected< OutPin1 >() == false ) )
        return;

    const InPin0& inputPin0 = getInputPin0();
    const InPin1& inputPin1 = getInputPin1();

    generator.addInclude( "includes/volume/csg/sphere.h" );

    String sphereVariableName = format::format( "sphereNode_{:x}", getId().get() );
    String sampleVariableName = format::format( "sphereSample_{:x}", getId().get() );
    String isoValueOutVariableName = format::format( "pin_{:x}", getOutputPin0().getGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", getOutputPin1().getGlobalId().get() );

    generator.appendCode( format::format(
        "CsgSphere {} = CsgSphere({}, {});\n",
        sphereVariableName,
        inputPin0.isConnected()
            ? format::format( "pin_{:x}", inputPin0.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin0 >() ),
        inputPin1.isConnected()
            ? format::format( "pin_{:x}", inputPin1.getLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::generateShaderValue( context.getPinData< typename Super::InPin1 >() ) ) );

    generator.appendCode( format::format( "vec4 {} = GetValueAndGradient(worldPosition, {});\n",
                                          sampleVariableName,
                                          sphereVariableName ) );
    generator.appendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, sampleVariableName ) );
    generator.appendCode( format::format( "vec3 {} = {}.xyz;\n", gradientOutVariableName, sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfSphereVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "Position";
    case InPin1::LocalId:
        return "Radius";
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
