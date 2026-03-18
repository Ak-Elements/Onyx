#pragma once

#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace onyx::volume
{
    class SdfDifferenceVolumeShaderGraphNode : public node_graph::FixedPinNode<graphics::ShaderGraphNode, 4, 2>
    {
    private:
        using Super = node_graph::FixedPinNode<graphics::ShaderGraphNode, 4, 2>;

        using InPinIsoValueSource0 = node_graph::Pin<onyxF32, "InPin0">;
        using InPinGradientSource0 = node_graph::Pin<Vector3f32, "InPin1">;
        using InPinIsoValueSource1 = node_graph::Pin<onyxF32, "InPin2">;
        using InPinGradientSource1 = node_graph::Pin<Vector3f32, "InPin3">;

        using OutPinGradient = node_graph::Pin<onyxF32, "OutPin0">;
        using OutPinIsoValue = node_graph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfDifferenceVolumeShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfDifferenceVolumeShaderGraphNode() = default;

        void OnUpdate(node_graph::ExecutionContext& context) const override;

        node_graph::PinBase* GetInputPin(onyxU32 index) override;
        const node_graph::PinBase* GetInputPin(onyxU32 index) const override;

        node_graph::PinBase* GetOutputPin(onyxU32 index) override;
        const node_graph::PinBase* GetOutputPin(onyxU32 index) const override;

    private:
        void DoGenerateShader(const node_graph::ExecutionContext& context, rhi::ShaderGenerator& generator) const override;

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
