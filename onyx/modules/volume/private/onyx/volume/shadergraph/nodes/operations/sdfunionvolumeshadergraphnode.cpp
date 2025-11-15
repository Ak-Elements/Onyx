#include <onyx/volume/shadergraph/nodes/operations/sdfunionvolumeshadergraphnode.h>

#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::Volume
{
    void SdfUnionVolumeShaderGraphNode::OnUpdate(NodeGraph::ExecutionContext& /*context*/) const
    {
    }

    void SdfUnionVolumeShaderGraphNode::DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != Graphics::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPinGradient>() == false) && (context.IsPinConnected<OutPinIsoValue>() == false))
            return;

        generator.AddInclude(Graphics::ShaderStage::All, "includes/volume/csg/operations/union.h");

        String unionVariableName = Format::Format("unionNode_{:x}", GetId().Get());
        
        String gradientOutVariableName = Format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());
        String isoValueOutVariableName = Format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());

        generator.AppendCode(Format::Format("vec4 {} = GetUnion(vec4({}, {}), vec4({}, {}));\n", unionVariableName,
            m_GradientSource0InPin.IsConnected() ? Format::Format("pin_{:x}", m_GradientSource0InPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinGradientSource0>()),
            m_IsoValueSource0InPin.IsConnected() ? Format::Format("pin_{:x}", m_IsoValueSource0InPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinIsoValueSource0>()),
            m_GradientSource1InPin.IsConnected() ? Format::Format("pin_{:x}", m_GradientSource1InPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinGradientSource1>()),
            m_IsoValueSource0InPin.IsConnected() ? Format::Format("pin_{:x}", m_IsoValueSource1InPin.GetLinkedPinGlobalId().Get()) : Graphics::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinIsoValueSource1>())));
        generator.AppendCode(Format::Format("float {} = {}.w;\n", isoValueOutVariableName, unionVariableName));
        generator.AppendCode(Format::Format("vec3 {} = {}.xyz;\n", gradientOutVariableName, unionVariableName));
    }

    NodeGraph::PinBase* SdfUnionVolumeShaderGraphNode::GetInputPin(onyxU32 index)
    {
        switch (index)
        {
        case 0: return &m_IsoValueSource0InPin;
        case 1: return &m_GradientSource0InPin;
        case 2: return &m_IsoValueSource1InPin;
        case 3: return &m_GradientSource1InPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    const NodeGraph::PinBase* SdfUnionVolumeShaderGraphNode::GetInputPin(onyxU32 index) const
    {
        switch (index)
        {
        case 0: return &m_IsoValueSource0InPin;
        case 1: return &m_GradientSource0InPin;
        case 2: return &m_IsoValueSource1InPin;
        case 3: return &m_GradientSource1InPin;
        }

        ONYX_ASSERT(false, "Invalid pin index");
        return nullptr;
    }

    NodeGraph::PinBase* SdfUnionVolumeShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        ONYX_ASSERT(index < GetOutputPinCount(), "Invalid pin index");

        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const NodeGraph::PinBase* SdfUnionVolumeShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        ONYX_ASSERT(index < GetOutputPinCount(), "Invalid pin index");
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

#if ONYX_IS_EDITOR
    StringView SdfUnionVolumeShaderGraphNode::GetPinName(StringId32 pinId) const
    {
        switch (pinId)
        {
        case InPinIsoValueSource0::LocalId: return "Iso Value A";
        case InPinGradientSource0::LocalId: return "Gradient A";
        case InPinIsoValueSource1::LocalId: return "Iso Value B";
        case InPinGradientSource1::LocalId: return "Gradient B";

        case OutPinIsoValue::LocalId: return "Iso Value";
        case OutPinGradient::LocalId: return "Gradient";
        }

        ONYX_ASSERT(false, "Invalid pin id");
        return "";
    }

#endif

}
