#pragma once
#include <onyx/assets/asset.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx::rhi
{
    class ShaderGenerator;
}

namespace onyx::file_system
{
    class FileStream;
}

namespace onyx::graphics
{
    class TextureAsset;
    class ShaderGraphNode;

    enum class ShaderPass
    {
        Surface, // PBR Material 
        PostProcess,
        UI,
        // Volume, Light etc.
    };

    enum class ShadingModel
    {
        Lit,
        Unlit,
        // Subsurface scattering etc.
    };

    class ShaderGraphTextures
    {
    public:
        onyxU32 AddTexture(const rhi::TextureHandle& texture);
        const DynamicArray<onyxU32>& GetTextures() const { return Textures; }
    private:
        DynamicArray<onyxU32> Textures;
    };

    class ShaderGraph : public assets::Asset<ShaderGraph>
    {
    public:
        node_graph::NodeGraph& GetNodeGraph() { return Graph; }
        const node_graph::NodeGraph& GetNodeGraph() const { return Graph; }

      //  void Render(const FrameContext& context, CommandBuffer& commandBuffer) const;

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        const String& GetShaderCode() const { return ShaderCode; }
        bool GenerateShader(rhi::ShaderGenerator& generator);
#endif

        bool Serialize(Serializer& serializer) const;
        bool Deserialize(const Deserializer& deserializer);

    private:
        virtual bool OnSerialize(Serializer& /*serializer*/) const { return true; }
        virtual bool OnDeserialize(const Deserializer& /*deserializer*/) { return true; }

    private:
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        String ShaderCode;
#endif

        node_graph::NodeGraph Graph;
    };
}
