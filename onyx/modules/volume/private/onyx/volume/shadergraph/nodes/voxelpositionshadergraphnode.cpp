#include <onyx/volume/shadergraph/nodes/voxelpositionshadergraphnode.h>

#include <onyx/graphicscore/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Volume
{
    GetVoxelPositionShaderGraphNode::GetVoxelPositionShaderGraphNode()
    {
        m_DebugName = "Voxel Position";
    }

    void GetVoxelPositionShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {
    }

    void GetVoxelPositionShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin>() == false))
            return;

        generator.AppendCode(Format::Format("vec3 pin_{:x} = voxelPosition;\n", m_Output.GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetVoxelPositionShaderGraphNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "Position";
    }
#endif
}
