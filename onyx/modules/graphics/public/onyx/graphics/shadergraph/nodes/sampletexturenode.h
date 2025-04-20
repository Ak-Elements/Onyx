#pragma once

#include <onyx/assets/asset.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/geometry/common.h>

namespace Onyx::Graphics
{
    struct TextureHandle;
    class TextureAsset;

    class SampleTextureNode : public NodeGraph::FlexiblePinsNode<ShaderGraphNode>
    {
        using Super = FlexiblePinsNode<ShaderGraphNode>;

        using TextureInPin = NodeGraph::Pin<TextureHandle, "Texture">;
        using UVInPin = NodeGraph::Pin<Vector2f, "UV">;
        using RGBOutPin = NodeGraph::Pin<Vector3f, "RGB">;
        using RGBAOutPin = NodeGraph::Pin<Vector4f, "RGBA">;
        using RedOutPin = NodeGraph::Pin<onyxF32, "R">;
        using GreenOutPin = NodeGraph::Pin<onyxF32, "G">;
        using BlueOutPin = NodeGraph::Pin<onyxF32, "B">;
        using AlphaOutPin = NodeGraph::Pin<onyxF32, "A">;

    public:
        SampleTextureNode();
        ~SampleTextureNode() override;

        void OnUpdate(NodeGraph::ExecutionContext& context) const override;

    private:
        bool OnSerialize(FileSystem::JsonValue& json) const override;
        bool OnDeserialize(const FileSystem::JsonValue& json) override;

        void DoGenerateShader(const NodeGraph::ExecutionContext& context, ShaderGenerator& generator) const override;
        void OnChanged(Assets::AssetSystem& assetSystem) override;

#if ONYX_IS_EDITOR
    protected:
        bool OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData) override;
        void OnUIDrawNode() override;
        StringView GetPinName(onyxU32 pinId) const override;
        NodeGraph::PinVisibility DoGetPinVisibility(onyxU32 localPinId) const override;
#endif
    private:
        // Texture asset id to use if input pin for texture handle is not connected
        Assets::AssetId TextureId;
        Reference<TextureAsset> Texture;
    };
}
