#include <onyx/volume/shadergraph/nodes/sdfnoiseshadergraphnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::volume
{
    void SdfNoise2DShaderGraphNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {

    }

    node_graph::PinBase* SdfNoise2DShaderGraphNode::GetInputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    const node_graph::PinBase* SdfNoise2DShaderGraphNode::GetInputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    node_graph::PinBase* SdfNoise2DShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const node_graph::PinBase* SdfNoise2DShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    void SdfNoise2DShaderGraphNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<IsoValueOutPin>() == false) && (context.IsPinConnected<GradientOutPin>() == false))
            return;

        String isoValueOutVariableName = format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());
        String gradientOutVariableName = format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());
        String gradientTmpVariableName = format::Format("noiseGradient_{:x}", GetId().Get());

        generator.AppendCode(format::Format("float {} = voxelPosition.y - {};\n", isoValueOutVariableName,
            m_NoiseValueInPin.IsConnected() ? format::Format("pin_{:x}", m_NoiseValueInPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseValueInPin>())));
        generator.AppendCode(format::Format("vec2 {} = {};\n", gradientTmpVariableName,
            m_NoiseGradientInPin.IsConnected() ? format::Format("pin_{:x}", m_NoiseGradientInPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseGradientInPin>())));

        generator.AppendCode(format::Format("vec3 {0} = normalize(vec3({1}.x, 1.0, {1}.y));\n", gradientOutVariableName, gradientTmpVariableName));
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

    void SdfNoise3DShaderGraphNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {

    }

    node_graph::PinBase* SdfNoise3DShaderGraphNode::GetInputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    const node_graph::PinBase* SdfNoise3DShaderGraphNode::GetInputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_NoiseValueInPin;

        return &m_NoiseGradientInPin;
    }

    node_graph::PinBase* SdfNoise3DShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const node_graph::PinBase* SdfNoise3DShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    void SdfNoise3DShaderGraphNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<IsoValueOutPin>() == false) && (context.IsPinConnected<GradientOutPin>() == false))
            return;

        String isoValueOutVariableName = format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());
        String gradientOutVariableName = format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());

        generator.AppendCode(format::Format("float {} = voxelPosition.y - {}.w;\n", isoValueOutVariableName,
            m_IsoValueOutPin.IsConnected() ? format::Format("pin_{:x}", m_IsoValueOutPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<IsoValueOutPin>())));
        generator.AppendCode(format::Format("vec3 {} = normalize({});\n", gradientOutVariableName,
            m_NoiseGradientInPin.IsConnected() ? format::Format("pin_{:x}", m_NoiseGradientInPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<NoiseGradientInPin>())));
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
