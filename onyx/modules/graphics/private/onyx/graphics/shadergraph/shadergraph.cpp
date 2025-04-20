#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/filesystem/filestream.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/shader/shadermodule.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/graphics/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>

namespace Onyx::Graphics
{
    onyxU32 ShaderGraphTextures::AddTexture(const TextureHandle& texture)
    {
        onyxU32 bindlessTextureIndex = texture.Texture->GetIndex();
        auto textureIt = std::ranges::find_if(Textures, [bindlessTextureIndex](onyxU32 textureIndex)
            {
                return bindlessTextureIndex == textureIndex;
            });

        if (textureIt != Textures.end())
        {
            return *textureIt;
        }

        onyxU32 textureEntryIndex = static_cast<onyxU32>(Textures.size()); // index in the texture constant buffer
        Textures.emplace_back(texture.Texture->GetIndex());
        return textureEntryIndex;
    }

    void ShaderGraph::Render(const FrameContext& /*context*/, CommandBuffer& commandBuffer) const
    {
        NodeGraph::GraphRunner runner(Graph);
        runner.Prepare();
        runner.Update(0);

        commandBuffer.BindShaderEffect(ShaderEffect);

        const ShaderGraphTextures& shaderTextures = runner.GetContext().Get<ShaderGraphTextures>();
        const DynamicArray<onyxU32>& textureIndices = shaderTextures.GetTextures();
        struct PushConstants
        {
            onyxU32 Textures[8];
        } constants;

        std::copy_n(textureIndices.data(), textureIndices.size(), constants.Textures);

        commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, sizeof(PushConstants), &constants);
    }

    bool ShaderGraph::Serialize(FileSystem::JsonValue& outJson) const
    {
        bool success = NodeGraph::Serializer::SerializeJson(Graph, outJson);

        if (success)
        {
            success = OnSerialize(outJson);
        }

        return success;
    }

    bool ShaderGraph::Deserialize(const FileSystem::JsonValue& inJson)
    {
        ShaderGraphNodeFactory factory;
        if (NodeGraph::Serializer::DeserializeJson(Graph, factory, inJson) == false)
        {
            return false;
        }

        if (OnDeserialize(inJson) == false)
        {
            return false;
        }

        return true;
    }

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR

    bool ShaderGraph::GenerateShader(ShaderGenerator& generator)
    {
        bool hasCompiled = Graph.Compile();

        if (hasCompiled == false)
            return false;

        NodeGraph::GraphRunner runner(Graph);

        // prepare nodes so data is setup
        runner.Prepare();

        // run one fake update to calculate all values
        runner.Update(0);

        ShaderGraphNodeFactory factory;
        const DynamicArray<onyxS8>& executionOrder = Graph.GetTopologicalOrder();
        NodeGraph::ExecutionContext& executionContext = runner.GetContext();
        for (onyxS8 localNodeId : executionOrder)
        {
            const ShaderGraphNode& node = Graph.GetNode<ShaderGraphNode>(localNodeId);
            executionContext.SetCurrentNode(node.GetId());

            generator.SetStage(ShaderStage::Fragment); //TODO: Add support for other stages

            generator.AppendCode(Format::Format("// {} 0x{:x} \n", node.GetName(), node.GetId().Get()));
            node.GenerateShader(executionContext, generator);
        }

        ShaderCode = generator.GenerateShader();
        ONYX_LOG_INFO("Resulting shader: {}", ShaderCode);
        return true;
    }

#endif
}
