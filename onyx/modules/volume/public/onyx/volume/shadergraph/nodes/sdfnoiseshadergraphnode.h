#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode2in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>

namespace Onyx::Volume
{
    class SdfNoise2DShaderGraphNode : public NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 2, 2>
    {
    private:
        using NoiseValueInPin = NodeGraph::Pin<onyxF32, "InPin0">;
        using NoiseGradientInPin = NodeGraph::Pin<Vector2f32, "InPin1">;
        using IsoValueOutPin = NodeGraph::Pin<onyxF32, "OutPin0">;
        using GradientOutPin = NodeGraph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::SdfNoise2DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfNoise2DShaderGraphNode() = default;

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
        NoiseValueInPin m_NoiseValueInPin;
        NoiseGradientInPin m_NoiseGradientInPin;
        IsoValueOutPin m_IsoValueOutPin;
        GradientOutPin m_GradientOutPin;
    };

    class SdfNoise3DShaderGraphNode : public NodeGraph::FixedPinNode<Graphics::ShaderGraphNode, 2, 2>
    {
    private:
        using NoiseValueInPin = NodeGraph::Pin<onyxF32, "InPin0">;
        using NoiseGradientInPin = NodeGraph::Pin<Vector3f32, "InPin1">;
        using IsoValueOutPin = NodeGraph::Pin<onyxF32, "OutPin0">;
        using GradientOutPin = NodeGraph::Pin<Vector3f32, "OutPin1">;

    public:
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::SdfNoise3DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SdfNoise3DShaderGraphNode() = default;

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
        NoiseValueInPin m_NoiseValueInPin;
        NoiseGradientInPin m_NoiseGradientInPin;
        IsoValueOutPin m_IsoValueOutPin;
        GradientOutPin m_GradientOutPin;
    };
}
