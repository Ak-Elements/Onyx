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

    void Reset() override;

    void Begin() override;
    void BeginSingleSubmit() override;
    void End() override;

    void BeginRenderPass( const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer ) override;
    void EndRenderPass() override;

    void BindShaderEffect( const ShaderInstanceHandle& shader ) override;
    void BindVertexBuffer( const BufferHandle& buffer, uint32_t binding, uint32_t offset ) override;
    void BindVertexBuffers( const InplaceArray< BufferHandle, 8 >& bufferHandles,
                            const InplaceArray< uint32_t, 8 > bufferOffsets,
                            uint32_t firstBinding,
                            uint32_t bindingCount ) override;
    void BindIndexBuffer( const BufferHandle& buffer, uint32_t offset, IndexType indexType ) override;

    void Bind( const TextureHandle& texture, const String& bindingName ) override;
    void Bind( const BufferHandle& buffer, const String& bindingName ) override;

    void Barrier( BufferHandle& buffer, Context newContext, Access newAccess ) override;
    void TransitionLayout( TextureHandle& texture,
                           Context newContext,
                           Access newAccess,
                           ImageLayout newLayout ) override;

    void SetViewport() override;
    void SetViewport( const Viewport& viewport ) override;
    void SetScissor() override;
    void SetScissor( Rect2s16 scissorRect ) override;

    void ClearColor( float32 red, float32 green, float32 blue, float32 alpha, uint32_t attachmentIndex ) override;
    void ClearDepthStencil( float32 depth, uint8_t stencil ) override;

    void Draw( PrimitiveTopology topology,
               uint32_t firstVertex,
               uint32_t vertexCount,
               uint32_t firstInstance,
               uint32_t instanceCount ) override;
    void DrawIndexed( PrimitiveTopology topology,
                      uint32_t indexCount,
                      uint32_t instanceCount,
                      uint32_t firstIndex,
                      int32_t vertexOffset,
                      uint32_t firstInstance ) override;
    void DrawIndirect( const BufferHandle& buffer, uint32_t drawCount, uint32_t offset, uint32_t stride ) override;
    void DrawIndirectCount( const BufferHandle& argumentBuffer,
                            uint32_t argumentOffset,
                            const BufferHandle& countBuffer,
                            uint32_t countOffset,
                            uint32_t maxDraws,
                            uint32_t stride ) override;
    void DrawIndexedIndirect( const BufferHandle& buffer,
                              uint32_t drawCount,
                              uint32_t offset,
                              uint32_t stride ) override;

    void DrawMeshTask( uint32_t taskCount, uint32_t firstTask ) override;
    void DrawMeshTaskIndirect( const BufferHandle& argumentBuffer,
                               uint32_t argumentOffset,
                               const BufferHandle& countBuffer,
                               uint32_t countOffset,
                               uint32_t maxDraws,
                               uint32_t stride ) override;

    void Dispatch( uint32_t groupX, uint32_t groupY, uint32_t groupZ ) override;
    void DispatchIndirect( const BufferHandle& buffer ) override;
    void DispatchIndirect( const BufferHandle& buffer, uint32_t offset ) override;

    void Copy( const BufferHandle& source, BufferHandle& destination ) override;

    // DEBUG
    void GlobalBarrier( VkAccessFlagBits2 srcAccess, VkAccessFlagBits2 dstAccess ) override;
    void GlobalBarrier( VkAccessFlagBits2 srcAccess,
                        VkPipelineStageFlags2 srcStage,
                        VkAccessFlagBits2 dstAccess,
                        VkPipelineStageFlags2 dstStage ) override;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void BeginDebugLabel( StringView label, const Vector4f32& color ) override;
    void EndDebugLabel() override;
#endif

  protected:
    void BindPushConstants( ShaderStage stage, uint32_t offset, uint32_t size, const void* data ) override;
    void BeginConditionalRendering( const BufferHandle& conditionalBuffer, uint32_t offset ) override;
    void EndConditionalRendering() override;

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
