#pragma once

#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    class ParticleSystemOutNode : public NodeGraph::FixedPinNode<ShaderGraphNode, 4, 0>
    {
    private:
        using PositionInPin = NodeGraph::Pin<Vector3f32, "Position">;
        using VelocityInPin = NodeGraph::Pin<Vector3f32, "Velocity">;
        using ColorInPin = NodeGraph::Pin<Vector3f32, "Color">;
        using LifeTimeInPin = NodeGraph::Pin<onyxF32, "LifeTime">;

    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::ParticleSystemOutNode";
        StringId32 GetTypeId() const override { return TypeId; }

        NodeGraph::PinBase* GetInputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetInputPin(onyxU32 index) const override;

        NodeGraph::PinBase* GetOutputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetOutputPin(onyxU32 index) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

    private:
        PositionInPin m_PositionInPin;
        VelocityInPin m_VelocityInPin;
        ColorInPin m_ColorInPin;
        LifeTimeInPin m_LifeTimeInPin;
    };
}
