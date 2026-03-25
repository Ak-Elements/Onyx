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
    virtual void Reset() = 0;
    virtual void Begin() = 0;
    virtual void BeginSingleSubmit() = 0;
    virtual void End() = 0;

    virtual void BeginRenderPass( const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer ) = 0;
    virtual void EndRenderPass() = 0;

    virtual void BindShaderEffect( const ShaderInstanceHandle& shader ) = 0;
    virtual void BindVertexBuffer( const BufferHandle& bufferHandle, uint32_t binding, uint32_t offset ) = 0;
    virtual void BindVertexBuffers( const InplaceArray< BufferHandle, 8 >& bufferHandles,
                                    const InplaceArray< uint32_t, 8 > bufferOffsets,
                                    uint32_t firstBinding,
                                    uint32_t bindingCount ) = 0;
    virtual void BindIndexBuffer( const BufferHandle& buffer, uint32_t offset, IndexType indexType ) = 0;

    template < typename T >
    void BindPushConstants( ShaderStage stage, const T& data ) {
        BindPushConstants( stage, 0, sizeof( T ), &data );
    }

    template < typename T >
    void BindPushConstants( ShaderStage stage, uint32_t offset, const T& data ) {
        BindPushConstants( stage, offset, sizeof( T ), &data );
    }

    template < typename T >
    void BindPushConstants( ShaderStage stage, uint32_t offset, const DynamicArray< T >& container ) {
        BindPushConstants( stage, offset, static_cast< uint32_t >( container.size() * sizeof( T ) ), container.data() );
    }

    virtual void Bind( const TextureHandle& texture, const String& bindingName ) = 0;
    virtual void Bind( const BufferHandle& buffer, const String& bindingName ) = 0;

    virtual void Barrier( BufferHandle& buffer, Context newContext, Access newAccess ) = 0;
    virtual void TransitionLayout( TextureHandle& texture,
                                   Context newContext,
                                   Access newAccess,
                                   ImageLayout newLayout ) = 0;

    virtual void SetViewport() = 0;
    virtual void SetViewport( const Viewport& viewport ) = 0;
    virtual void SetScissor() = 0;
    virtual void SetScissor( Rect2s16 scissorRect ) = 0;

    virtual void ClearColor( float32 red, float32 green, float32 blue, float32 alpha, uint32_t attachmentIndex ) = 0;
    virtual void ClearDepthStencil( float32 depth, uint8_t stencil ) = 0;

    virtual void Draw( PrimitiveTopology topology,
                       uint32_t firstVertex,
                       uint32_t vertexCount,
                       uint32_t firstInstance,
                       uint32_t instanceCount ) = 0;
    virtual void DrawIndexed( PrimitiveTopology topology,
                              uint32_t indexCount,
                              uint32_t instanceCount,
                              uint32_t firstIndex,
                              int32_t vertexOffset,
                              uint32_t firstInstance ) = 0;
    virtual void DrawIndirect( const BufferHandle& buffer, uint32_t drawCount, uint32_t offset, uint32_t stride ) = 0;
    virtual void DrawIndirectCount( const BufferHandle& argumentBuffer,
                                    uint32_t argumentOffset,
                                    const BufferHandle& countBuffer,
                                    uint32_t countOffset,
                                    uint32_t maxDraws,
                                    uint32_t stride ) = 0;
    virtual void DrawIndexedIndirect( const BufferHandle& buffer,
                                      uint32_t drawCount,
                                      uint32_t offset,
                                      uint32_t stride ) = 0;

    virtual void DrawMeshTask( uint32_t taskCount, uint32_t firstTask ) = 0;
    virtual void DrawMeshTaskIndirect( const BufferHandle& argumentBuffer,
                                       uint32_t argumentOffset,
                                       const BufferHandle& countBuffer,
                                       uint32_t countOffset,
                                       uint32_t maxDraws,
                                       uint32_t stride ) = 0;

    virtual void Dispatch( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) = 0;
    virtual void DispatchIndirect( const BufferHandle& buffer ) = 0;
    virtual void DispatchIndirect( const BufferHandle& buffer, uint32_t offset ) = 0;

    virtual void Copy( const BufferHandle& source, BufferHandle& destination ) = 0;

    // DEBUG
    virtual void GlobalBarrier( uint64_t srcAccess, uint64_t dstAccess ) = 0;
    virtual void GlobalBarrier( uint64_t srcAccess, uint64_t srcStage, uint64_t dstAccess, uint64_t dstStage ) = 0;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    virtual void BeginDebugLabel( StringView label, const Vector4f32& color ) = 0;
    virtual void EndDebugLabel() = 0;
#endif

  protected:
    virtual void BindPushConstants( ShaderStage stage, uint32_t offset, uint32_t size, const void* data ) = 0;

    virtual void BeginConditionalRendering( const BufferHandle& conditionalBuffer, uint32_t offset ) = 0;
    virtual void EndConditionalRendering() = 0;
};

struct ConditionalRender {
    ConditionalRender( CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer )
        : m_CommandBuffer( &commandBuffer ) {
        commandBuffer.BeginConditionalRendering( conditionalBuffer, 0 );
    }

    ConditionalRender( CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer, uint32_t offset )
        : m_CommandBuffer( &commandBuffer ) {
        commandBuffer.BeginConditionalRendering( conditionalBuffer, offset );
    }

    ~ConditionalRender() { m_CommandBuffer->EndConditionalRendering(); }

  private:
    CommandBuffer* m_CommandBuffer = nullptr;
};
} // namespace onyx::rhi
