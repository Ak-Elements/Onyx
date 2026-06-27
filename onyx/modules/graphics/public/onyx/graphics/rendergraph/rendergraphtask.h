#pragma once

#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/rendergraph/rendergraphresource.h>
#include <onyx/nodegraph/nodes/node.h>
#include <onyx/rhi/buffer.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/pipeline.h>
#include <onyx/rhi/renderpass.h>
#include <onyx/rhi/texturestorage.h>

namespace onyx::rhi {
class CommandBuffer;
struct FrameContext;
class GraphicsSystem;
} // namespace onyx::rhi

namespace onyx::graphics {
class RenderGraph;

// rename to render graph task
struct RenderGraphPolicy {
    using RenderGraphResourceCache = HashMap< RenderGraphResourceId, RenderGraphResource >;

  public:
    virtual void init( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) = 0;
    virtual void shutdown( onyx::rhi::GraphicsSystem& api ) = 0;

    virtual void compile( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) = 0;

    virtual void beginFrame( RenderGraphContext& context ) = 0;

    virtual void preRender( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) = 0;
    virtual void render( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) = 0;
    virtual void postRender( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) = 0;

    virtual void endFrame( RenderGraphContext& context ) = 0;

    // TODO: Find better names for both. IsEnabled -> should this node be started this frame
    // HasBegunFrame - Has this node started this frame
    [[nodiscard]] virtual bool isEnabled() const = 0;
    [[nodiscard]] virtual bool hasBegunFrame() const = 0;

    virtual void onSwapChainResized( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) = 0;
};

class IRenderGraphNode : public node_graph::NodeWithPolicy< RenderGraphPolicy > {
  public:
    void init( onyx::rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/ ) override {}
    void shutdown( onyx::rhi::GraphicsSystem& /*api*/ ) override {}
    void compile( onyx::rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/ ) override {}
    void beginFrame( RenderGraphContext& /*context*/ ) override {}
    void preRender( RenderGraphContext& /*context*/, onyx::rhi::CommandBuffer& /*commandBuffer*/ ) override {}
    void render( RenderGraphContext& /*context*/, onyx::rhi::CommandBuffer& /*commandBuffer*/ ) override {}
    void postRender( RenderGraphContext& /*context*/, onyx::rhi::CommandBuffer& /*commandBuffer*/ ) override {}
    void endFrame( RenderGraphContext& /*context*/ ) override {}

    [[nodiscard]] bool isEnabled() const override { return false; }
    [[nodiscard]] bool hasBegunFrame() const override { return false; }

    void onSwapChainResized( onyx::rhi::GraphicsSystem& /*api*/,
                             RenderGraphResourceCache& /*resourceCache*/ ) override {}

  protected:
    template < typename T >
    T& getGraphInput( RenderGraph& renderGraph ) {
        return renderGraph.getInput< T >();
    }

    template < typename T >
    const T& getGraphInput( RenderGraph& renderGraph ) const {
        return renderGraph.getInput< T >();
    }

    template < typename T >
    void addGraphInput( RenderGraph& renderGraph ) {
        renderGraph.addInput< T >();
    }
};

class RenderGraphShaderNode : public IRenderGraphNode {
  public:
    void init( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) override;
    void shutdown( onyx::rhi::GraphicsSystem& api ) final;

    void compile( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) override;

    void beginFrame( RenderGraphContext& context ) override;

    void preRender( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) final;
    void render( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) override;
    void postRender( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) final;

    void endFrame( RenderGraphContext& context ) final;

    bool OnSerialize( Serializer& serializer ) const override;
    bool OnDeserialize( const Deserializer& deserializer ) override;

    const RenderGraphTextureResourceInfo& getInputResourceInfo( uint32_t pinIndex ) {
        return pinIndex >= m_inputAttachmentInfos.size() ? m_inputAttachmentInfos.emplace_back()
                                                         : m_inputAttachmentInfos[ pinIndex ];
    }
    const RenderGraphTextureResourceInfo& getOuputResourceInfo( uint32_t pinIndex ) {
        return pinIndex >= m_outputAttachmentInfos.size() ? m_outputAttachmentInfos.emplace_back()
                                                          : m_outputAttachmentInfos[ pinIndex ];
    }

    void onSwapChainResized( onyx::rhi::GraphicsSystem& /*api*/, RenderGraphResourceCache& /*resourceCache*/ ) override;

    virtual bool isComputeTask() const { return false; }

    bool hasBegunFrame() const override { return m_hasBegunFrame; }

  protected:
#if ONYX_IS_EDITOR
    bool OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) override;
#endif

    void bindResources( rhi::ShaderInstanceHandle shaderInstance,
                        const RenderGraphResourceCache& resourceCache,
                        const onyx::rhi::FrameContext& frameContext );

  protected:
    virtual void onInit( onyx::rhi::GraphicsSystem&, RenderGraphResourceCache& ) {}
    virtual void onShutdown( onyx::rhi::GraphicsSystem& ) {}

    virtual void onBeginFrame( RenderGraphContext& ) {}

    virtual void onPreRender( RenderGraphContext&, onyx::rhi::CommandBuffer& ) {}
    virtual void onRender( RenderGraphContext&, onyx::rhi::CommandBuffer& ) {}
    virtual void onPostRender( RenderGraphContext&, onyx::rhi::CommandBuffer& ) {}

    virtual void onEndFrame( RenderGraphContext& ) {}

    void createRenderPass( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache );
    void updateFramebuffer( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache );

  protected:
    rhi::RenderPassHandle m_renderPass;
    rhi::FramebufferHandle m_framebuffer;

    DynamicArray< RenderGraphTextureResourceInfo > m_inputAttachmentInfos;

    DynamicArray< RenderGraphTextureResourceInfo > m_outputAttachmentInfos;
    DynamicArray< RenderGraphBufferResourceInfo > m_outputBufferInfos;

    bool m_hasBegunFrame = false;
};

class RenderGraphFixedShaderNode : public RenderGraphShaderNode {
  public:
    void init( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) final;

    void compile( onyx::rhi::GraphicsSystem& api, RenderGraphResourceCache& resourceCache ) override;

    void beginFrame( RenderGraphContext& context ) final;
    void render( RenderGraphContext& context, onyx::rhi::CommandBuffer& commandBuffer ) override;

    bool isComputeTask() const override {
        ONYX_ASSERT( m_shaderInstance.isValid() );
        return m_shaderInstance->IsCompute();
    }
    bool isEnabled() const override { return m_shaderInstance.isValid(); }

    bool OnSerialize( Serializer& serializer ) const override;
    bool OnDeserialize( const Deserializer& deserializer ) override;

  private:
#if ONYX_IS_EDITOR
    bool OnDrawInPropertyGrid( HashMap< Guid64, std::any >& constantPinData ) override;
#endif
  protected:
    rhi::PipelineProperties m_pipelineProperties;
    rhi::ShaderInstanceHandle m_shaderInstance;
};

} // namespace onyx::graphics
