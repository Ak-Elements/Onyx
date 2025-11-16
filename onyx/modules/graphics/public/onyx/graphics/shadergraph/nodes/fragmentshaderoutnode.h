#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in.h>
#include <onyx/nodegraph/nodes/fixedpinsnode.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/pins/pin.h>

namespace Onyx::Graphics
{
    class FragmentShaderOutNode : public NodeGraph::FixedPinNode_1_In<ShaderGraphNode, Vector4f32>
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::FragmentShaderOut";
        StringId32 GetTypeId() const override { return TypeId; }

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    private:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };

    class PBRMaterialShaderOutNode : public NodeGraph::FixedPinNode<ShaderGraphNode, 4, 0>
    {
    private:
        using AlbedoInPin = NodeGraph::Pin<Vector3f32, "Albedo">;
        using NormalInPin = NodeGraph::Pin<Vector3f32, "Normal">;
        using MetalnessInPin = NodeGraph::Pin<onyxF32, "Metalness">;
        using RoughnessInPin = NodeGraph::Pin<onyxF32, "Roughness">;

        using Super = NodeGraph::FixedPinNode<ShaderGraphNode, 4, 0>;
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::PBRMaterialShaderOut";
        StringId32 GetTypeId() const override { return TypeId; }

        NodeGraph::PinBase* GetInputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetInputPin(onyxU32 index) const override;

        NodeGraph::PinBase* GetOutputPin(onyxU32 index) override;
        const NodeGraph::PinBase* GetOutputPin(onyxU32 index) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(StringId32 pinId) const override;
#endif
    private:
        AlbedoInPin m_AlbedoInPin;
        NormalInPin m_NormalInPin;
        MetalnessInPin m_MetalnessInPin;
        RoughnessInPin m_RoughnessInPin;
    };
}
