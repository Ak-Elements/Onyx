#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx::graphics
{
    onyxU32 ShaderGraphTextures::AddTexture(const rhi::TextureHandle& texture)
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

    // void ShaderGraph::Render(const FrameContext& /*context*/, CommandBuffer& commandBuffer) const
    // {
    //    NodeGraph::GraphRunner runner(Graph);
    //    runner.Prepare();
    //    runner.Update(0);

    //    commandBuffer.BindShaderEffect(m_ShaderEffect);

    //    const ShaderGraphTextures& shaderTextures = runner.GetContext().Get<ShaderGraphTextures>();
    //    const DynamicArray<onyxU32>& textureIndices = shaderTextures.GetTextures();
    //    struct PushConstants
    //    {
    //        onyxU32 Textures[8];
    //    } constants;

    //    std::copy_n(textureIndices.data(), textureIndices.size(), constants.Textures);

    //    commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, constants);
    // }

    bool ShaderGraph::Serialize(Serializer& serializer) const
    {
        bool success = node_graph::Serialize(serializer, Graph);

        if (success)
        {
           success = OnSerialize(serializer);
        }

        return success;
    }

    bool ShaderGraph::Deserialize(const Deserializer& deserializer)
    {
        ShaderGraphNodeFactory factory;
        if (node_graph::Deserialize(deserializer, Graph, factory) == false)
        {
            return false;
        }

        if (OnDeserialize(deserializer) == false)
        {
            return false;
        }

        return true;
    }

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR

    bool ShaderGraph::GenerateShader(rhi::ShaderGenerator& generator)
    {
        bool hasCompiled = Graph.Compile();

        if (hasCompiled == false)
            return false;

        node_graph::GraphRunner runner(Graph);

        // prepare nodes so data is setup
        runner.Prepare();

        // run one fake update to calculate all values
        runner.Update(0);

        ShaderGraphNodeFactory factory;
        const DynamicArray<onyxS8>& executionOrder = Graph.GetTopologicalOrder();
        node_graph::ExecutionContext& executionContext = runner.GetContext();
        for (onyxS8 localNodeId : executionOrder)
        {
            const ShaderGraphNode& node = Graph.GetNode<ShaderGraphNode>(localNodeId);
            executionContext.SetCurrentNode(node.GetId());

            generator.SetStage(rhi::ShaderStage::Fragment); //TODO: Add support for other stages

            generator.AppendCode(format::Format("// {} 0x{:x} \n", node.GetName(), node.GetId().Get()));
            node.GenerateShader(executionContext, generator);
        }

        ShaderCode = generator.GenerateShader();
        return true;
    }

#endif
}
