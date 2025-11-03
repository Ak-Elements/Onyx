#include <onyx/graphics/shadergraph/nodes/math/simplexnoisenode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Graphics
{
    SimplexNoise2DShaderGraphNode::SimplexNoise2DShaderGraphNode()
    {
        m_DebugName = "Simplex Noise 2D";
    }

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
        
        generator.AddInclude(ShaderStage::All, "includes/math/simplexnoise2d.h");

        String noiseVariable = Format::Format("noiseNode_{:x}", GetId().Get());
        String outPin0VariableName = Format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String outPin1VariableName = Format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(Format::Format("vec3 {} = snoise({});\n", noiseVariable,
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>())));

        generator.AppendCode(Format::Format("vec2 {} = {}.xy;\n", outPin0VariableName, noiseVariable));
        generator.AppendCode(Format::Format("float {} = {}.z;\n", outPin1VariableName, noiseVariable));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise2DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case InPin::LocalId: return "Position";
        case OutPin0::LocalId: return "Gradient";
        case OutPin1::LocalId: return "Noise Value";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif

    SimplexNoise3DShaderGraphNode::SimplexNoise3DShaderGraphNode()
    {
        m_DebugName = "Simplex Noise 3D";
    }

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

        generator.AddInclude(ShaderStage::All, "includes/math/simplexnoise3d.h");

        String noiseVariable = Format::Format("noiseNode_{:x}", GetId().Get());
        String outPin0VariableName = Format::Format("pin_{:x}", GetOutputPin0().GetGlobalId().Get());
        String outPin1VariableName = Format::Format("pin_{:x}", GetOutputPin1().GetGlobalId().Get());

        generator.AppendCode(Format::Format("vec4 {} = snoise({});\n", noiseVariable,
            inputPin0.IsConnected() ? Format::Format("pin_{:x}", inputPin0.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<typename Super::InPin>())));

        generator.AppendCode(Format::Format("vec3 {} = {}.xy;\n", outPin0VariableName, noiseVariable));
        generator.AppendCode(Format::Format("float {} = {}.z;\n", outPin1VariableName, noiseVariable));
    }

#if ONYX_IS_EDITOR
    StringView SimplexNoise3DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
            case InPin::LocalId: return "Position";
            case OutPin0::LocalId: return "Gradient";
            case OutPin1::LocalId: return "Noise Value";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif
}
