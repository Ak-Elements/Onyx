#pragma once

#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::Volume
{
    class SdfIntersectVolumeShaderGraphNode : public NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 4, 2>
    {
    private:
        using Super = NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 4, 2>;

        using InPinIsoValueSource0 = NodeGraph::Pin<onyxF32, "InPin0">;
        using InPinGradientSource0 = NodeGraph::Pin<Vector3f32, "InPin1">;
        using InPinIsoValueSource1 = NodeGraph::Pin<onyxF32, "InPin2">;
        using InPinGradientSource1 = NodeGraph::Pin<Vector3f32, "InPin3">;

        using OutPinIsoValue = NodeGraph::Pin<onyxF32, "OutPin0">;
        using OutPinGradient = NodeGraph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::SdfIntersectVolumeShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfIntersectVolumeShaderGraphNode() = default;

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

        NodeGraph::PinBase* GetInputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetInputPin(onyxU32 index) const override;

        NodeGraph::PinBase* GetOutputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetOutputPin(onyxU32 index) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif

    private:
        InPinIsoValueSource0 m_IsoValueSource0InPin;
        InPinGradientSource0 m_GradientSource0InPin;
        InPinIsoValueSource1 m_IsoValueSource1InPin;
        InPinGradientSource1 m_GradientSource1InPin;
        OutPinIsoValue m_IsoValueOutPin;
        OutPinGradient m_GradientOutPin;
    };
}
