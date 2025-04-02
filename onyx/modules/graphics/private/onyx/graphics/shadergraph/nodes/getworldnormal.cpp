#include <onyx/graphics/shadergraph/nodes/getworldnormal.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics
{
    void GetWorldNormalNode::DoGenerateShader(const NodeGraph::ExecutionContext& /*context*/, ShaderGenerator& generator) const
    {
        if (generator.GetStage() != ShaderStage::Fragment)
        {
            return;
        }

        generator.AppendCode(Format::Format("vec4 pin_{:x} = vec4(WorldNormal, 1.0); \n", GetOutputPin().GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetWorldNormalNode::GetPinName(onyxU32 /*pinId*/) const
    {
        return "World Normal";
    }
#endif

}
