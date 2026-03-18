#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

namespace onyx::graphics::shader_graph_nodes
{
    void GetWorldNormalNode::DoGenerateShader(const node_graph::ExecutionContext& /*context*/, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
        {
            return;
        }

        generator.AppendCode(format::Format("vec4 pin_{:x} = vec4(worldNormal, 1.0); \n", GetOutputPin().GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetWorldNormalNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "World Normal";
    }
#endif

}
