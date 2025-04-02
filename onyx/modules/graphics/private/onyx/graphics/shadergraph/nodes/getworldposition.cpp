#include <onyx/graphics/shadergraph/nodes/getworldposition.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    void GetWorldPositionNode::DoGenerateShader(const NodeGraph::ExecutionContext& /*context*/, ShaderGenerator& generator) const
    {
        if (generator.GetStage() != ShaderStage::Fragment)
        {
            return;
        }

        generator.AppendCode(Format::Format("vec4 pin_{:x} = vec4(WorldPosition, 1.0); \n", GetOutputPin().GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetWorldPositionNode::GetPinName(onyxU32 /*pinId*/) const
    {
        return "World Position";
    }
#endif

}
