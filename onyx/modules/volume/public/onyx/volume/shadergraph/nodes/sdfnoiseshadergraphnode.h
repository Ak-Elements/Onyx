#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>

namespace onyx::volume
{
    class SdfNoise2DShaderGraphNode : public node_graph::FixedPinNode<graphics::ShaderGraphNode, 2, 2>
    {
    private:
        using NoiseValueInPin = node_graph::Pin<onyxF32, "InPin0">;
        using NoiseGradientInPin = node_graph::Pin<Vector2f32, "InPin1">;
        using IsoValueOutPin = node_graph::Pin<onyxF32, "OutPin0">;
        using GradientOutPin = node_graph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfNoise2DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfNoise2DShaderGraphNode() = default;

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
        NoiseValueInPin m_NoiseValueInPin;
        NoiseGradientInPin m_NoiseGradientInPin;
        IsoValueOutPin m_IsoValueOutPin;
        GradientOutPin m_GradientOutPin;
    };

    class SdfNoise3DShaderGraphNode : public node_graph::FixedPinNode<graphics::ShaderGraphNode, 2, 2>
    {
    private:
        using NoiseValueInPin = node_graph::Pin<onyxF32, "InPin0">;
        using NoiseGradientInPin = node_graph::Pin<Vector3f32, "InPin1">;
        using IsoValueOutPin = node_graph::Pin<onyxF32, "OutPin0">;
        using GradientOutPin = node_graph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "onyx::volume::volume_shader_graph::SdfNoise3DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfNoise3DShaderGraphNode() = default;

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
        NoiseValueInPin m_NoiseValueInPin;
        NoiseGradientInPin m_NoiseGradientInPin;
        IsoValueOutPin m_IsoValueOutPin;
        GradientOutPin m_GradientOutPin;
    };
}
