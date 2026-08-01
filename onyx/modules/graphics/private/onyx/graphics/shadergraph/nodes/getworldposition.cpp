#include <onyx/graphics/shadergraph/nodes/getworldposition.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void GetWorldPositionNode::doGenerateShader( const node_graph::ExecutionContext& /*context*/,
                                             rhi::ShaderGenerator& generator ) const {
    if( generator.getStage() != rhi::ShaderStage::Fragment ) {
        return;
    }

    generator.appendCode(
        format::format( "vec4 pin_{:x} = vec4(worldPosition, 1.0); \n", getOutputPin().getGlobalId().get() ) );
}

#if ONYX_IS_EDITOR
StringView GetWorldPositionNode::getPinName( StringId32 /*pinId*/ ) const {
    return "World Position";
}
#endif

} // namespace onyx::graphics::shader_graph_nodes
