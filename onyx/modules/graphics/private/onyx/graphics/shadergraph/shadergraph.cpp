#include <onyx/graphics/shadergraph/shadergraph.h>

#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/nodegraph/graphrunner.h>
#include <onyx/nodegraph/nodegraphserializer.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/shader/generators/shadergenerator.h>

#include <onyx/graphics/shadergraph/shadergraphnode.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::graphics {
uint32_t ShaderGraphTextures::AddTexture( const rhi::TextureHandle& texture ) {
    uint32_t bindlessTextureIndex = texture.Texture->GetIndex();
    auto textureIt = std::ranges::find_if( Textures, [ bindlessTextureIndex ]( uint32_t textureIndex ) {
        return bindlessTextureIndex == textureIndex;
    } );

    if( textureIt != Textures.end() ) {
        return *textureIt;
    }

    uint32_t textureEntryIndex = static_cast< uint32_t >( Textures.size() ); // index in the texture constant buffer
    Textures.emplace_back( texture.Texture->GetIndex() );
    return textureEntryIndex;
}

// void ShaderGraph::Render(const FrameContext& /*context*/, CommandBuffer& commandBuffer) const
// {
//    NodeGraph::GraphRunner runner(Graph);
//    runner.Prepare();
//    runner.Update(0);

//    commandBuffer.BindShaderEffect(m_ShaderEffect);

//    const ShaderGraphTextures& shaderTextures = runner.GetContext().Get<ShaderGraphTextures>();
//    const DynamicArray<uint32_t>& textureIndices = shaderTextures.GetTextures();
//    struct PushConstants
//    {
//        uint32_t Textures[8];
//    } constants;

//    std::copy_n(textureIndices.data(), textureIndices.size(), constants.Textures);

//    commandBuffer.BindPushConstants(ShaderStage::Fragment, 0, constants);
// }

bool ShaderGraph::serialize( Serializer& serializer ) const {
    bool success = node_graph::serialize( serializer, Graph );

    if( success ) {
        success = OnSerialize( serializer );
    }

    return success;
}

bool ShaderGraph::deserialize( const Deserializer& deserializer ) {
    ShaderGraphNodeFactory factory;
    if( node_graph::deserialize( deserializer, Graph, factory ) == false ) {
        return false;
    }

    if( OnDeserialize( deserializer ) == false ) {
        return false;
    }

    return true;
}

#if !ONYX_IS_RELEASE || ONYX_IS_EDITOR

bool ShaderGraph::GenerateShader( rhi::ShaderGenerator& generator ) {
    bool hasCompiled = Graph.compile();

    if( hasCompiled == false )
        return false;

    node_graph::GraphRunner runner( Graph );

    // prepare nodes so data is setup
    runner.Prepare();

    // run one fake update to calculate all values
    runner.Update( 0 );

    ShaderGraphNodeFactory factory;
    const DynamicArray< int8_t >& executionOrder = Graph.getTopologicalOrder();
    node_graph::ExecutionContext& executionContext = runner.GetContext();
    for( int8_t localNodeId : executionOrder ) {
        const ShaderGraphNode& node = Graph.getNode< ShaderGraphNode >( localNodeId );
        executionContext.SetCurrentNode( node.GetId() );

        generator.SetStage( rhi::ShaderStage::Fragment ); // TODO: Add support for other stages

        generator.AppendCode( format::format( "// {} 0x{:x} \n", node.GetName(), node.GetId().get() ) );
        node.GenerateShader( executionContext, generator );
    }

    ShaderCode = generator.GenerateShader();
    return true;
}

#endif
} // namespace onyx::graphics
