#include <onyx/graphics/shadergraph/nodes/getdeltatime.h>

#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    void GetDeltaTimeNode::DoGenerateShader(const NodeGraph::ExecutionContext& /*context*/, ShaderGenerator& generator) const
    {
        generator.AppendCode(Format::Format("float pin_{:x} = constants.DeltaTime;\n", GetOutputPin().GetGlobalId().Get()));
    }

#if ONYX_IS_EDITOR
    StringView GetDeltaTimeNode::GetPinName(StringId32 /*pinId*/) const
    {
        return "Seconds";
    }
#endif

}
