#pragma once

#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::Volume
{
    class IntersectVolumeShaderGraphNode : public NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 4, 2>
    {
    private:
        using Super = NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 4, 2>;

        using InPinGradientSource0 = NodeGraph::Pin<Vector3f32, "InPin0">;
        using InPinIsoValueSource0 = NodeGraph::Pin<onyxF32, "InPin1">;
        using InPinGradientSource1 = NodeGraph::Pin<Vector3f32, "InPin2">;
        using InPinIsoValueSource1 = NodeGraph::Pin<onyxF32, "InPin3">;

        using OutPinGradient = NodeGraph::Pin<Vector3f32, "OutPin0">;
        using OutPinIsoValue = NodeGraph::Pin<onyxF32, "OutPin1">;

    public:
        IntersectVolumeShaderGraphNode();
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::IntersectVolumeShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

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
        InPinGradientSource0 m_GradientSource0InPin;
        InPinIsoValueSource0 m_IsoValueSource0InPin;
        InPinGradientSource1 m_GradientSource1InPin;
        InPinIsoValueSource1 m_IsoValueSource1InPin;
        OutPinGradient m_GradientOutPin;
        OutPinIsoValue m_IsoValueOutPin;

    };
}
