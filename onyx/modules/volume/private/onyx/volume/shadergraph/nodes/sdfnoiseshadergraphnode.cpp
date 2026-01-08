#include <onyx/volume/shadergraph/nodes/sdfnoiseshadergraphnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Volume
{
    void SdfNoise2DShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {

    }

    NodeGraph::PinBase* SdfNoise2DShaderGraphNode::GetInputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    const NodeGraph::PinBase* SdfNoise2DShaderGraphNode::GetInputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    NodeGraph::PinBase* SdfNoise2DShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const NodeGraph::PinBase* SdfNoise2DShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    void SdfNoise2DShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<IsoValueOutPin>() == false) && (context.IsPinConnected<GradientOutPin>() == false))
            return;

        String isoValueOutVariableName = Format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());
        String gradientOutVariableName = Format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());
        String gradientTmpVariableName = Format::Format("noiseGradient_{:x}", GetId().Get());

        generator.AppendCode(Format::Format("float {} = voxelPosition.y - {};\n", isoValueOutVariableName,
            m_NoiseValueInPin.IsConnected() ? Format::Format("pin_{:x}", m_NoiseValueInPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseValueInPin>())));
        generator.AppendCode(Format::Format("vec2 {} = {};\n", gradientTmpVariableName,
            m_NoiseGradientInPin.IsConnected() ? Format::Format("pin_{:x}", m_NoiseGradientInPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseGradientInPin>())));

        generator.AppendCode(Format::Format("vec3 {0} = normalize(vec3({1}.x, 1.0, {1}.y));\n", gradientOutVariableName, gradientTmpVariableName));
    }

#if ONYX_IS_EDITOR
    StringView SdfNoise2DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case NoiseValueInPin::LocalId: return "Noise";
        case NoiseGradientInPin::LocalId: return "Noise Gradient";
        case IsoValueOutPin::LocalId: return "Iso Value";
        case GradientOutPin::LocalId: return "Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif

    void SdfNoise3DShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {

    }

    NodeGraph::PinBase* SdfNoise3DShaderGraphNode::GetInputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    const NodeGraph::PinBase* SdfNoise3DShaderGraphNode::GetInputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    NodeGraph::PinBase* SdfNoise3DShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const NodeGraph::PinBase* SdfNoise3DShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    void SdfNoise3DShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<IsoValueOutPin>() == false) && (context.IsPinConnected<GradientOutPin>() == false))
            return;

        String isoValueOutVariableName = Format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());
        String gradientOutVariableName = Format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());

        generator.AppendCode(Format::Format("float {} = voxelPosition.y - {}.w;\n", isoValueOutVariableName,
            m_IsoValueOutPin.IsConnected() ? Format::Format("pin_{:x}", m_IsoValueOutPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<IsoValueOutPin>())));
        generator.AppendCode(Format::Format("vec3 {} = normalize({});\n", gradientOutVariableName,
            m_NoiseGradientInPin.IsConnected() ? Format::Format("pin_{:x}", m_NoiseGradientInPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseGradientInPin>())));
    }

#if ONYX_IS_EDITOR
    StringView SdfNoise3DShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case NoiseValueInPin::LocalId: return "Noise";
        case NoiseGradientInPin::LocalId: return "Noise Gradient";
        case IsoValueOutPin::LocalId: return "Iso Value";
        case GradientOutPin::LocalId: return "Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }
#endif
}
