#pragma once

#include <onyx/nodegraph/nodes/fixedpinnode1in2out.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    class SimplexNoise2DShaderGraphNode : public NodeGraph::FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector2f32, Vector2f32, onyxF32>
    {
    private:
        using Super = NodeGraph::FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector2f32, Vector2f32, onyxF32>;
    public:
        SimplexNoise2DShaderGraphNode();
        static constexpr StringId32 TypeId = "Onyx::Graphics::ShaderGraph::SimplexNoise2DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };

    class SimplexNoise3DShaderGraphNode : public NodeGraph::FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector3f32, Vector3f32, onyxF32>
    {
    private:
        using Super = NodeGraph::FixedPinNode_1_In_2_Out<Graphics::ShaderGraphNode, Vector3f32, Vector3f32, onyxF32>;
    public:
        SimplexNoise3DShaderGraphNode();
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeShaderGraph::SimplexNoise3DShaderGraphNode";
        StringId32 GetTypeId() const override { return TypeId; }

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        void DoGenerateShader(const NodeGraph::ExecutionContext& context, Graphics::ShaderGenerator& generator) const override;

#if ONYX_IS_EDITOR
    protected:
        StringView GetPinName(StringId32 pinId) const override;
#endif
    };
}