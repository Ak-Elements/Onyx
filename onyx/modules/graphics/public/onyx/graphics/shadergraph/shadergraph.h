#pragma once
#include <onyx/assets/asset.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi {
class ShaderGenerator;
}

namespace onyx::file_system {
class FileStream;
}

namespace onyx::graphics {
class TextureAsset;
class ShaderGraphNode;

enum class ShaderPass : uint8_t {
    Surface, // PBR Material
    PostProcess,
    UI,
    // Volume, Light etc.
};

enum class ShadingModel : uint8_t {
    Lit,
    Unlit,
    // Subsurface scattering etc.
};

class ShaderGraphTextures {
  public:
    uint32_t addTexture( const rhi::TextureHandle& texture );
    [[nodiscard]] const DynamicArray< uint32_t >& getTextures() const { return m_textures; }

  private:
    DynamicArray< uint32_t > m_textures;
};

class ShaderGraph : public assets::Asset< ShaderGraph > {
  public:
    node_graph::NodeGraph& getNodeGraph() { return m_graph; }
    const node_graph::NodeGraph& getNodeGraph() const { return m_graph; }

    //  void Render(const FrameContext& context, CommandBuffer& commandBuffer) const;

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    const String& getShaderCode() const { return m_shaderCode; }
    bool generateShader( rhi::ShaderGenerator& generator );
#endif

    bool serialize( Serializer& serializer ) const;
    bool deserialize( const Deserializer& deserializer );

  private:
    virtual bool onSerialize( Serializer& /*serializer*/ ) const { return true; }
    virtual bool onDeserialize( const Deserializer& /*deserializer*/ ) { return true; }

  private:
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
    String m_shaderCode;
#endif

    node_graph::NodeGraph m_graph;
};
} // namespace onyx::graphics
