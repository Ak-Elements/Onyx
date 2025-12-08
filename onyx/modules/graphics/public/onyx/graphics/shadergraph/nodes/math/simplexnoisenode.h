#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics::ShaderGraphNodes
{
    class SimplexNoise2DNode : public NodeGraph::FixedPinNode_1_In_2_Out<ShaderGraphNode, Vector2f32, onyxF32, Vector2f32>
    {
    private:
        using Super = NodeGraph::FixedPinNode_1_In_2_Out<ShaderGraphNode, Vector2f32, onyxF32, Vector2f32>;
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::SimplexNoise2DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SimplexNoise2DNode() = default;

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };

    class SimplexNoise3DNode : public NodeGraph::FixedPinNode_1_In_2_Out<ShaderGraphNode, Vector3f32, onyxF32, Vector3f32>
    {
    private:
        using Super = NodeGraph::FixedPinNode_1_In_2_Out<ShaderGraphNode, Vector3f32, onyxF32, Vector3f32>;
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::SimplexNoise3DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        SimplexNoise3DNode() = default;

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}