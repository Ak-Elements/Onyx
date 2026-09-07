#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes {
void GetWorldNormalNode::doGenerateShader( const node_graph::ExecutionContext& /*context*/,
                                           rhi::ShaderGenerator& generator ) const {
    // TODO:
    // if( generator.getStage() != rhi::ShaderStage::Fragment ) {
    //     return;
    // }

    generator.appendCode(
        format::format( "float4 pin_{:x} = float4(worldNormal, 0.0); \n", getOutputPin().getGlobalId().get() ) );
}

#if ONYX_IS_EDITOR
StringView GetWorldNormalNode::getPinName( StringId32 /*pinId*/ ) const {
    return "World Normal";
}
#endif

} // namespace onyx::graphics::shader_graph_nodes
