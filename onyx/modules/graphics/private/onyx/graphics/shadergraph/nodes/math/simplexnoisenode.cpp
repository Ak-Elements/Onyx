#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::graphics::shader_graph_nodes
{
    void SimplexNoise2DNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {
    }

    void SimplexNoise2DNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin& inputPin0 = GetInputPin();
        
        generator.AddInclude(rhi::ShaderStage::All, "includes/math/psrdnoise2d.h");

        String noiseVariable = format::Format("noiseNode_{:x}", GetId().Get());
        String noiseValueOutVariableName = format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String noiseGradientOutVariableName = format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(format::Format("vec2 {};\n", noiseGradientOutVariableName));
        generator.AppendCode(format::Format("float {} = psrdnoise({}, vec2(0.0f,0.0f), 0.0f, {});\n", noiseValueOutVariableName,
            inputPin0.IsConnected() ? format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>()), 
            noiseGradientOutVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise2DNode::GetPinName(StringId32 pinId) const
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

    void SimplexNoise3DNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {

    }

    void SimplexNoise3DNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPin0>() == false) && (context.IsPinConnected<OutPin1>() == false))
            return;

        const InPin& inputPin0 = GetInputPin();

        generator.AddInclude(rhi::ShaderStage::All, "includes/math/psrdnoise2d.h");

        String noiseVariable = format::Format("noiseNode_{:x}", GetId().Get());
        String noiseValueOutVariableName = format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String noiseGradientOutVariableName = format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(format::Format("vec3 {};\n", noiseGradientOutVariableName));
        generator.AppendCode(format::Format("float {} = psrdnoise({}, vec3(0.0f,0.0f, 0.0f), 0.0f, {});\n", noiseValueOutVariableName,
            inputPin0.IsConnected() ? format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>()),
            noiseGradientOutVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise3DNode::GetPinName(StringId32 pinId) const
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
