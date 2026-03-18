#include <onyx/volume/shadergraph/nodes/operations/sdfintersectvolumeshadergraphnode.h>

#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/graphics/shadergraph/shadergraph.h>
#include <onyx/nodegraph/executioncontext.h>

namespace onyx::volume
{
    void SdfIntersectVolumeShaderGraphNode::OnUpdate(node_graph::ExecutionContext& /*context*/) const
    {
    }

    void SdfIntersectVolumeShaderGraphNode::DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const
    {
        if (generator.GetStage() != rhi::ShaderStage::Fragment)
            return;

        if ((context.IsPinConnected<OutPinGradient>() == false) && (context.IsPinConnected<OutPinIsoValue>() == false))
            return;

        generator.AddInclude(rhi::ShaderStage::All, "includes/volume/csg/operations/intersect.h");

        String intersectVariableName = format::Format("intersectNode_{:x}", GetId().Get());

        String gradientOutVariableName = format::Format("pin_{:x}", m_GradientOutPin.GetGlobalId().Get());
        String isoValueOutVariableName = format::Format("pin_{:x}", m_IsoValueOutPin.GetGlobalId().Get());

        generator.AppendCode(format::Format("vec4 {} = GetIntersection(vec4({}, {}), vec4({}, {}));\n", intersectVariableName,
            m_GradientSource0InPin.IsConnected() ? format::Format("pin_{:x}", m_GradientSource0InPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinGradientSource0>()),
            m_IsoValueSource0InPin.IsConnected() ? format::Format("pin_{:x}", m_IsoValueSource0InPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinIsoValueSource0>()),
            m_GradientSource1InPin.IsConnected() ? format::Format("pin_{:x}", m_GradientSource1InPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinGradientSource1>()),
            m_IsoValueSource0InPin.IsConnected() ? format::Format("pin_{:x}", m_IsoValueSource1InPin.GetLinkedPinGlobalId().Get()) : rhi::ShaderGenerator::GenerateShaderValue(context.GetPinData<InPinIsoValueSource1>())));
        generator.AppendCode(format::Format("float {} = {}.w;\n", isoValueOutVariableName, intersectVariableName));
        generator.AppendCode(format::Format("vec3 {} = {}.xyz;\n", gradientOutVariableName, intersectVariableName));
    }


    node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::GetInputPin(onyxU32 index)
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

    const node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::GetInputPin(onyxU32 index) const
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

    node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::GetOutputPin(onyxU32 index)
    {
        ONYX_ASSERT(index < GetOutputPinCount(), "Invalid pin index");

        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

    const node_graph::PinBase* SdfIntersectVolumeShaderGraphNode::GetOutputPin(onyxU32 index) const
    {
        ONYX_ASSERT(index < GetOutputPinCount(), "Invalid pin index");
        if (index == 0)
            return &m_IsoValueOutPin;

        return &m_GradientOutPin;
    }

#if ONYX_IS_EDITOR
    StringView SdfIntersectVolumeShaderGraphNode::GetPinName(StringId32 pinId) const
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
