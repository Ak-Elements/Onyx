#pragma once

#include <onyx/assets/assethandle.h>
#include <onyx/assets/assetid.h>
#include <onyx/graphics/shadergraph/shadergraphnode.h>
#include <onyx/nodegraph/nodes/flexiblepinsnode.h>

namespace onyx::rhi {
struct TextureHandle;
}

namespace onyx::graphics {
class TextureAsset;

namespace shader_graph_nodes {
class SampleTextureNode : public node_graph::FlexiblePinsNode< ShaderGraphNode > {
  private:
    using Super = node_graph::FlexiblePinsNode< ShaderGraphNode >;

    using TextureInPin = node_graph::Pin< rhi::TextureHandle, "Texture" >;
    using UVInPin = node_graph::Pin< Vector2f32, "UV" >;
    using RGBOutPin = node_graph::Pin< Vector3f32, "RGB" >;
    using RGBAOutPin = node_graph::Pin< Vector4f32, "RGBA" >;
    using RedOutPin = node_graph::Pin< float32, "R" >;
    using GreenOutPin = node_graph::Pin< float32, "G" >;
    using BlueOutPin = node_graph::Pin< float32, "B" >;
    using AlphaOutPin = node_graph::Pin< float32, "A" >;

  public:
    static constexpr StringId32 TypeId = "onyx::graphics::shader_graph_nodes::SampleTexture";
    StringId32 GetTypeId() const override { return TypeId; }

    SampleTextureNode();
    ~SampleTextureNode() override;

    void OnUpdate( node_graph::ExecutionContext& context ) const override;

  private:
    bool OnSerialize( Serializer& serializer ) const override;
    bool OnDeserialize( const Deserializer& deserializer ) override;

    void DoGenerateShader( const node_graph::ExecutionContext& context,
                           rhi::ShaderGenerator& generator ) const override;
    void OnChanged( assets::AssetSystem& assetSystem ) override;

#if ONYX_IS_EDITOR
  protected:
    // bool OnDrawInPropertyGrid(HashMap<Guid64, std::any>& constantPinData) override;
    // void OnUIDrawNode() override;
    StringView GetPinName( StringId32 pinId ) const override;
    node_graph::PinVisibility DoGetPinVisibility( StringId32 localPinId ) const override;
#endif
  private:
    // Texture asset id to use if input pin for texture handle is not connected
    assets::AssetHandle< TextureAsset > Texture;
};
} // namespace shader_graph_nodes
} // namespace onyx::graphics