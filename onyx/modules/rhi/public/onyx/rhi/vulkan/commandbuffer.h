#pragma once

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicstypes.h>
#include <onyx/rhi/vulkan/graphicsapi.h>
#include <onyx/rhi/vulkan/vulkan.h>

namespace onyx::rhi::vulkan {
class VulkanFramebuffer;
class CommandPool;
class Device;
class Pipeline;
class VulkanRenderPass;

class VulkanCommandBuffer : public CommandBuffer {
  public:
    VulkanCommandBuffer( const VulkanGraphicsApi& api,
                         const CommandPool& commandPool,
                         VkCommandBufferLevel bufferLevel,
                         uint8_t frameIndex,
                         StringView debugName );
    ~VulkanCommandBuffer();

    VulkanCommandBuffer( VulkanCommandBuffer&& );

    void reset() override;

    void begin() override;
    void beginSingleSubmit() override;
    void end() override;

    void beginRenderPass( const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer ) override;
    void endRenderPass() override;

    void bindShaderEffect( const ShaderInstanceHandle& shader ) override;
    void bindVertexBuffer( const BufferHandle& buffer, uint32_t binding, uint32_t offset ) override;
    void bindVertexBuffers( const InplaceArray< BufferHandle, 8 >& bufferHandles,
                            const InplaceArray< uint32_t, 8 > bufferOffsets,
                            uint32_t firstBinding,
                            uint32_t bindingCount ) override;
    void bindIndexBuffer( const BufferHandle& buffer, uint32_t offset, IndexType indexType ) override;

    void bind( const TextureHandle& texture, const String& bindingName ) override;
    void bind( const BufferHandle& buffer, const String& bindingName ) override;

    void barrier( BufferHandle& buffer, Context newContext, Access newAccess ) override;
    void transitionLayout( TextureHandle& texture,
                           Context newContext,
                           Access newAccess,
                           ImageLayout newLayout ) override;

    void setViewport() override;
    void setViewport( const Viewport& viewport ) override;
    void setScissor() override;
    void setScissor( Rect2s16 scissorRect ) override;

    void clearColor( float32 red, float32 green, float32 blue, float32 alpha, uint32_t attachmentIndex ) override;
    void clearDepthStencil( float32 depth, uint8_t stencil ) override;

    void draw( PrimitiveTopology topology,
               uint32_t firstVertex,
               uint32_t vertexCount,
               uint32_t firstInstance,
               uint32_t instanceCount ) override;
    void drawIndexed( PrimitiveTopology topology,
                      uint32_t indexCount,
                      uint32_t instanceCount,
                      uint32_t firstIndex,
                      int32_t vertexOffset,
                      uint32_t firstInstance ) override;
    void drawIndirect( const BufferHandle& buffer, uint32_t drawCount, uint32_t offset, uint32_t stride ) override;
    void drawIndirectCount( const BufferHandle& argumentBuffer,
                            uint32_t argumentOffset,
                            const BufferHandle& countBuffer,
                            uint32_t countOffset,
                            uint32_t maxDraws,
                            uint32_t stride ) override;
    void drawIndexedIndirect( const BufferHandle& buffer,
                              uint32_t drawCount,
                              uint32_t offset,
                              uint32_t stride ) override;

    void drawMeshTask( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) override;
    void drawMeshTaskIndirect( const BufferHandle& argumentBuffer,
                               uint32_t argumentOffset,
                               const BufferHandle& countBuffer,
                               uint32_t countOffset,
                               uint32_t maxDraws,
                               uint32_t stride ) override;

    void dispatch( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) override;
    void dispatchIndirect( const BufferHandle& buffer ) override;
    void dispatchIndirect( const BufferHandle& buffer, uint32_t offset ) override;

    void copy( const BufferHandle& source, BufferHandle& destination ) override;

    // DEBUG
    void globalBarrier( VkAccessFlagBits2 srcAccess, VkAccessFlagBits2 dstAccess ) override;
    void globalBarrier( VkAccessFlagBits2 srcAccess,
                        VkPipelineStageFlags2 srcStage,
                        VkAccessFlagBits2 dstAccess,
                        VkPipelineStageFlags2 dstStage ) override;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void beginDebugLabel( StringView label, const Vector4f32& color ) override;
    void endDebugLabel() override;
#endif

  protected:
    void bindPushConstants( ShaderStage stage, uint32_t offset, uint32_t size, const void* data ) override;
    void beginConditionalRendering( const BufferHandle& conditionalBuffer, uint32_t offset ) override;
    void endConditionalRendering() override;

  private:
    void PreDraw();
    void BindDescriptorSets( VkPipelineLayout pipelineLayout, VkPipelineBindPoint bindingPoint );

    VkPipelineLayout GetPipelineLayout() const;

  private:
    const VulkanGraphicsApi& m_Api;

    Reference< VulkanRenderPass > m_CurrentRenderPass;
    Reference< VulkanFramebuffer > m_CurrentFrameBuffer;

    ShaderInstanceHandle m_CurrentShaderEffect;

    VULKAN_HANDLE( VkCommandBuffer, CommandBuffer, nullptr );

    uint8_t m_FrameIndex;

    bool m_IsRecording = false;
};
} // namespace onyx::rhi::vulkan
