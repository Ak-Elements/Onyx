#include <onyx/volume/shadergraph/nodes/voxelpositionshadergraphnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::volume
{
    GetVoxelPositionShaderGraphNode::GetVoxelPositionShaderGraphNode()
    {
        m_DebugName = "Voxel Position";
    }

    void GetVoxelPositionShaderGraphNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {
    }

    void GetVoxelPositionShaderGraphNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin>() == false))
            return;

        generator.AppendCode(format::Format("vec3 pin_{:x} = voxelPosition;\n", m_Output.GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetVoxelPositionShaderGraphNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "Position";
    }
#endif
}
