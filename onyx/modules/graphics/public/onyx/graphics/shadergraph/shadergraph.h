#pragma once
#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/assets/assetserializer.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Graphics
{
    class ShaderGenerator;
}

namespace Onyx::FileSystem
{
    class FileStream;
}

namespace Onyx::Graphics
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
        onyxU32 AddTexture(const TextureHandle& texture);
        const DynamicArray<onyxU32>& GetTextures() const { return Textures; }
    private:
        DynamicArray<onyxU32> Textures;
    };

    class ShaderGraph : public Assets::Asset<ShaderGraph>
    {
    public:
        NodeGraph::NodeGraph& GetNodeGraph() { return Graph; }
        const NodeGraph::NodeGraph& GetNodeGraph() const { return Graph; }

        ShaderEffectHandle& GetShaderEffect() { return ShaderEffect; }
        const ShaderEffectHandle& GetShaderEffect() const { return ShaderEffect; }

        void Render(const FrameContext& context, CommandBuffer& commandBuffer) const;

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        const String& GetShaderCode() const { return ShaderCode; }
        bool GenerateShader(ShaderGenerator& generator);

#endif

        bool Serialize(FileSystem::JsonValue& outJson) const;
        bool Deserialize(const FileSystem::JsonValue& outJson);

    private:
        virtual bool OnSerialize(FileSystem::JsonValue& /*outJson*/) const { return true; }
        virtual bool OnDeserialize(const FileSystem::JsonValue& /*outJson*/) { return true; }

    private:
#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR
        String ShaderCode;
#endif

        // Blendmode
        // Cullmode

        NodeGraph::NodeGraph Graph;
        ShaderEffectHandle ShaderEffect;
    };
}
