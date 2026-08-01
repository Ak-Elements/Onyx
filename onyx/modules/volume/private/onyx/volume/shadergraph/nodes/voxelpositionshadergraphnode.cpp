#include <onyx/volume/shadergraph/nodes/voxelpositionshadergraphnode.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::volume {
GetVoxelPositionShaderGraphNode::GetVoxelPositionShaderGraphNode() {
    m_debugName = "Voxel Position";
}

void GetVoxelPositionShaderGraphNode::onUpdate( node_graph::ExecutionContext& /*context*/ ) const {}

void GetVoxelPositionShaderGraphNode::doGenerateShader( const node_graph::ExecutionContext& context,
                                                        rhi::ShaderGenerator& generator ) const {
    if( generator.getStage() != rhi::ShaderStage::Fragment )
        return;

    if( ( context.isPinConnected< OutPin >() == false ) )
        return;

    generator.appendCode( format::format( "vec3 pin_{:x} = worldPosition;\n", m_output.getGlobalId().get() ) );
}

#if ONYX_IS_EDITOR
StringView GetVoxelPositionShaderGraphNode::getPinName( StringId32 /*pinId*/ ) const {
    return "Position";
}
#endif
} // namespace onyx::volume
