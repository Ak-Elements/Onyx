#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/graphicstypes.h>

namespace onyx::rhi {
struct TextureHandle;
class Buffer;
class Pipeline;
class Framebuffer;
class RenderPass;

class CommandBuffer : public NonCopyable {
    friend struct ConditionalRender;

  public:
    virtual void reset() = 0;
    virtual void begin() = 0;
    virtual void beginSingleSubmit() = 0;
    virtual void end() = 0;

    virtual void beginRenderPass( const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer ) = 0;
    virtual void endRenderPass() = 0;

    virtual void bindShaderEffect( const ShaderInstanceHandle& shader ) = 0;
    virtual void bindVertexBuffer( const BufferHandle& bufferHandle, uint32_t binding, uint32_t offset ) = 0;
    virtual void bindVertexBuffers( const InplaceArray< BufferHandle, 8 >& bufferHandles,
                                    const InplaceArray< uint32_t, 8 > bufferOffsets,
                                    uint32_t firstBinding,
                                    uint32_t bindingCount ) = 0;
    virtual void bindIndexBuffer( const BufferHandle& buffer, uint32_t offset, IndexType indexType ) = 0;

    template < typename T >
    void bindPushConstants( ShaderStage stage, const T& data ) {
        bindPushConstants( stage, 0, sizeof( T ), &data );
    }

    template < typename T >
    void bindPushConstants( ShaderStage stage, uint32_t offset, const T& data ) {
        bindPushConstants( stage, offset, sizeof( T ), &data );
    }

    template < typename T >
    void bindPushConstants( ShaderStage stage, uint32_t offset, const DynamicArray< T >& container ) {
        bindPushConstants( stage, offset, static_cast< uint32_t >( container.size() * sizeof( T ) ), container.data() );
    }

    virtual void bind( const TextureHandle& texture, const String& bindingName ) = 0;
    virtual void bind( const BufferHandle& buffer, const String& bindingName ) = 0;

    virtual void barrier( BufferHandle& buffer, Context newContext, Access newAccess ) = 0;
    virtual void transitionLayout( TextureHandle& texture,
                                   Context newContext,
                                   Access newAccess,
                                   ImageLayout newLayout ) = 0;

    virtual void setViewport() = 0;
    virtual void setViewport( const Viewport& viewport ) = 0;
    virtual void setScissor() = 0;
    virtual void setScissor( Rect2s16 scissorRect ) = 0;

    virtual void clearColor( float32 red, float32 green, float32 blue, float32 alpha, uint32_t attachmentIndex ) = 0;
    virtual void clearDepthStencil( float32 depth, uint8_t stencil ) = 0;

    virtual void draw( PrimitiveTopology topology,
                       uint32_t firstVertex,
                       uint32_t vertexCount,
                       uint32_t firstInstance,
                       uint32_t instanceCount ) = 0;
    virtual void drawIndexed( PrimitiveTopology topology,
                              uint32_t indexCount,
                              uint32_t instanceCount,
                              uint32_t firstIndex,
                              int32_t vertexOffset,
                              uint32_t firstInstance ) = 0;
    virtual void drawIndirect( const BufferHandle& buffer, uint32_t drawCount, uint32_t offset, uint32_t stride ) = 0;
    virtual void drawIndirectCount( const BufferHandle& argumentBuffer,
                                    uint32_t argumentOffset,
                                    const BufferHandle& countBuffer,
                                    uint32_t countOffset,
                                    uint32_t maxDraws,
                                    uint32_t stride ) = 0;
    virtual void drawIndexedIndirect( const BufferHandle& buffer,
                                      uint32_t drawCount,
                                      uint32_t offset,
                                      uint32_t stride ) = 0;

    virtual void drawMeshTask( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) = 0;
    virtual void drawMeshTaskIndirect( const BufferHandle& argumentBuffer,
                                       uint32_t argumentOffset,
                                       const BufferHandle& countBuffer,
                                       uint32_t countOffset,
                                       uint32_t maxDraws,
                                       uint32_t stride ) = 0;

    virtual void dispatch( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) = 0;
    virtual void dispatchIndirect( const BufferHandle& buffer ) = 0;
    virtual void dispatchIndirect( const BufferHandle& buffer, uint32_t offset ) = 0;

    virtual void copy( const BufferHandle& source, BufferHandle& destination ) = 0;

    // DEBUG
    virtual void globalBarrier( uint64_t srcAccess, uint64_t dstAccess ) = 0;
    virtual void globalBarrier( uint64_t srcAccess, uint64_t srcStage, uint64_t dstAccess, uint64_t dstStage ) = 0;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    virtual void beginDebugLabel( StringView label, const Vector4f32& color ) = 0;
    virtual void endDebugLabel() = 0;
#endif

  protected:
    virtual void bindPushConstants( ShaderStage stage, uint32_t offset, uint32_t size, const void* data ) = 0;

    virtual void beginConditionalRendering( const BufferHandle& conditionalBuffer, uint32_t offset ) = 0;
    virtual void endConditionalRendering() = 0;
};

struct ConditionalRender {
    ConditionalRender( CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer )
        : m_commandBuffer( &commandBuffer ) {
        commandBuffer.beginConditionalRendering( conditionalBuffer, 0 );
    }

    ConditionalRender( CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer, uint32_t offset )
        : m_commandBuffer( &commandBuffer ) {
        commandBuffer.beginConditionalRendering( conditionalBuffer, offset );
    }

    ~ConditionalRender() { m_commandBuffer->endConditionalRendering(); }

  private:
    CommandBuffer* m_commandBuffer = nullptr;
};
} // namespace onyx::rhi
