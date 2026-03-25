#include <onyx/graphics/shadergraph/nodes/getworldposition.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void GetWorldPositionNode::DoGenerateShader( const node_graph::ExecutionContext& /*context*/,
                                             rhi::ShaderGenerator& generator ) const {
    if ( generator.GetStage() != rhi::ShaderStage::Fragment ) {
        return;
    }

    generator.AppendCode(
        format::format( "vec4 pin_{:x} = vec4(worldPosition, 1.0); \n", GetOutputPin().GetGlobalId().get() ) );
}

#if ONYX_IS_EDITOR
StringView GetWorldPositionNode::GetPinName( StringId32 /*pinId*/ ) const {
    return "World Position";
}
#endif

} // namespace onyx::graphics::shader_graph_nodes
