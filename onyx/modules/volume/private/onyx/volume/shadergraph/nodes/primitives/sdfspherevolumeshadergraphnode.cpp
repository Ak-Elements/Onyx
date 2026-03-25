#include <onyx/volume/shadergraph/nodes/primitives/sdfspherevolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
void SdfSphereVolumeShaderGraphNode::OnUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void SdfSphereVolumeShaderGraphNode::DoGenerateShader( const node_graph::ExecutionContext& context,
                                                       rhi::ShaderGenerator& generator ) const {
    if ( generator.GetStage() != rhi::ShaderStage::Fragment )
        return;

    if ( ( context.IsPinConnected< OutPin0 >() == false ) && ( context.IsPinConnected< OutPin1 >() == false ) )
        return;

    const InPin0& inputPin0 = GetInputPin0();
    const InPin1& inputPin1 = GetInputPin1();

    generator.AddInclude( rhi::ShaderStage::All, "includes/volume/csg/sphere.h" );

    String sphereVariableName = format::format( "sphereNode_{:x}", GetId().get() );
    String sampleVariableName = format::format( "sphereSample_{:x}", GetId().get() );
    String isoValueOutVariableName = format::format( "pin_{:x}", GetOutputPin0().GetGlobalId().get() );
    String gradientOutVariableName = format::format( "pin_{:x}", GetOutputPin1().GetGlobalId().get() );

    generator.AppendCode( format::format(
        "CsgSphere {} = CsgSphere({}, {});\n",
        sphereVariableName,
        inputPin0.IsConnected()
            ? format::format( "pin_{:x}", inputPin0.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< typename Super::InPin0 >() ),
        inputPin1.IsConnected()
            ? format::format( "pin_{:x}", inputPin1.GetLinkedPinGlobalId().get() )
            : rhi::ShaderGenerator::GenerateShaderValue( context.GetPinData< typename Super::InPin1 >() ) ) );

    generator.AppendCode( format::format( "vec4 {} = GetValueAndGradient(worldPosition, {});\n",
                                          sampleVariableName,
                                          sphereVariableName ) );
    generator.AppendCode( format::format( "float {} = {}.w;\n", isoValueOutVariableName, sampleVariableName ) );
    generator.AppendCode( format::format( "vec3 {} = {}.xyz;\n", gradientOutVariableName, sampleVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfSphereVolumeShaderGraphNode::GetPinName( StringId32 pinId ) const {
    switch ( pinId ) {
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
