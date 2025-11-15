#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Graphics
{
    void SimplexNoise2DShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {

    }

    void SimplexNoise2DShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        if (generator.GetStage() != ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin& inputPin0 = GetInputPin();
        
        generator.AddInclude(ShaderStage::All, "includes/math/psrdnoise2d.h");

        String noiseVariable = Format::Format("noiseNode_{:x}", GetId().Get());
        String noiseValueOutVariableName = Format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String noiseGradientOutVariableName = Format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(Format::Format("vec2 {};\n", noiseGradientOutVariableName));
        generator.AppendCode(Format::Format("float {} = psrdnoise({}, vec2(0.0f,0.0f), 0.0f, {});\n", noiseValueOutVariableName,
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>()), 
            noiseGradientOutVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise2DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case InPin::LocalId: return "Position";
        case OutPin0::LocalId: return "Noise Value";
        case OutPin1::LocalId: return "Noise Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif

    void SimplexNoise3DShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {

    }

    void SimplexNoise3DShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const
    {
        if (generator.GetStage() != ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin& inputPin0 = GetInputPin();

        generator.AddInclude(ShaderStage::All, "includes/math/psrdnoise2d.h");

        String noiseVariable = Format::Format("noiseNode_{:x}", GetId().Get());
        String noiseValueOutVariableName = Format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String noiseGradientOutVariableName = Format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(Format::Format("vec3 {};\n", noiseGradientOutVariableName));
        generator.AppendCode(Format::Format("float {} = psrdnoise({}, vec3(0.0f,0.0f, 0.0f), 0.0f, {});\n", noiseValueOutVariableName,
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>()),
            noiseGradientOutVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise3DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin::LocalId: return "Position";
            case OutPin0::LocalId: return "Noise Value";
            case OutPin1::LocalId: return "Noise Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif
}
