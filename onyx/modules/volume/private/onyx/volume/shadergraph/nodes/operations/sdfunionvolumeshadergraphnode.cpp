#include <onyx/volume/shadergraph/nodes/operations/sdfunionvolumeshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
constexpr StringView UnionCode = R"(
        SdfSample {0};
        if( {1}.Distance < {2}.Distance ) {{
            {0}.Gradient = {1}.Gradient;
            {0}.Distance = {1}.Distance;
        }} else {{
            {0}.Gradient = {2}.Gradient;
            {0}.Distance = {2}.Distance;
        }}
    )";

void SdfUnionVolumeShaderGraphNode::onUpdate( node_graph::ExecutionContext& context ) const {
    const SdfSample& lhsSample = context.getPinData< InPin0 >();
    const SdfSample& rhsSample = context.getPinData< InPin1 >();

    SdfSample& outSample = context.getPinData< OutPin >();
    outSample = lhsSample.Distance < rhsSample.Distance ? lhsSample : rhsSample;
}

void SdfUnionVolumeShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                      rhi::ShaderGenerator& generator ) const {
    if( context.isPinConnected< OutPin >() == false ) {
        return;
    }

    generator.addImport( "includes.volume.csg.operations.union" );

    String unionVariableName = format::format( "unionNode_{:x}", getId().get() );

    String lhsVariableName = format::format( "pin_{:x}", getInputPin0().getLinkedPinGlobalId().get() );
    String rhsVariableName = format::format( "pin_{:x}", getInputPin1().getLinkedPinGlobalId().get() );
    String outVariableName = format::format( "pin_{:x}", getOutputPin().getGlobalId().get() );

    generator.appendCode( format::format( UnionCode, outVariableName, lhsVariableName, rhsVariableName ) );
}

#if ONYX_IS_EDITOR
StringView SdfUnionVolumeShaderGraphNode::getPinName( StringId32 pinId ) const {
    switch( pinId ) {
    case InPin0::LocalId:
        return "A";
    case InPin1::LocalId:
        return "B";
    case OutPin::LocalId:
        return "Sdf Sample";
    }

    ONYX_ASSERT( false, "Invalid pin id" );
    return "";
}

#endif

} // namespace onyx::volume
