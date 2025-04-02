#pragma once

#include <onyx/graphics/commandbuffer.h>
#include <onyx/graphics/graphicstypes.h>
#include <onyx/graphics/vulkan/graphicsapi.h>
#include <onyx/graphics/vulkan/vulkan.h>

namespace Onyx::Graphics::Vulkan
{
    class VulkanFramebuffer;
    class CommandPool;
    class Device;
    class Pipeline;
    class VulkanRenderPass;

    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(const VulkanGraphicsApi& api, const CommandPool& commandPool, VkCommandBufferLevel bufferLevel, onyxU8 frameIndex, const StringView& debugName);
        ~VulkanCommandBuffer();

        VulkanCommandBuffer(VulkanCommandBuffer&&);
        
        void Reset() override;

        void Begin() override;
        void BeginSingleSubmit() override;
        void End() override;

        void BeginRenderPass(const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer) override;
        void EndRenderPass() override;

        void BindShaderEffect(const ShaderEffectHandle& shaderEffect) override;
        void BindVertexBuffer(const BufferHandle& buffer, onyxU32 binding, onyxU32 offset) override;
        void BindVertexBuffers(const InplaceArray<BufferHandle, 8>& bufferHandles, const InplaceArray<onyxU32, 8> bufferOffsets, onyxU32 firstBinding, onyxU32 bindingCount) override;
        void BindIndexBuffer(const BufferHandle& buffer, onyxU32 offset, IndexType indexType) override;
        void BindPushConstants(ShaderStage stage, onyxU32 offset, onyxU32 size, void* data) override;

        void Bind(const TextureHandle& texture, const String& bindingName) override;
        void Bind(const BufferHandle& buffer, const String& bindingName) override;

        void SetViewport() override;
        void SetViewport(const Viewport& viewport) override;
        void SetScissor() override;
        void SetScissor(const Rect2Ds16& scissorRect) override;

        void ClearColor(onyxF32 red, onyxF32 green, onyxF32 blue, onyxF32 alpha, onyxU32 attachmentIndex) override;
        void ClearDepthStencil(onyxF32 depth, onyxU8 stencil) override;

        void Draw(PrimitiveTopology topology, onyxU32 firstVertex, onyxU32 vertexCount, onyxU32 firstInstance, onyxU32 instanceCount) override;
        void DrawIndexed(PrimitiveTopology topology, onyxU32 indexCount, onyxU32 instanceCount, onyxU32 firstIndex, onyxS32 vertexOffset, onyxU32 firstInstance) override;
        void DrawIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) override;
        void DrawIndirectCount(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) override;
        void DrawIndexedIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) override;

        void DrawMeshTask(onyxU32 taskCount, onyxU32 firstTask) override;
        void DrawMeshTaskIndirect(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) override;

        void Dispatch(onyxU32 groupX, onyxU32 groupY, onyxU32 groupZ) override;
        void DispatchIndirect(const BufferHandle& buffer, onyxU32 offset) override;

        void Copy(const BufferHandle& source, BufferHandle& destination) override;

        // DEBUG
        void GlobalBarrier(VkAccessFlagBits2 srcAccess, VkAccessFlagBits2 dstAccess) override;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        void BeginDebugLabel(const StringView& label, const Vector4f& color) override;
        void EndDebugLabel() override;
#endif

    protected:
        void PreDraw();
        void BindDescriptorSets() override;
        void BindPipeline(const PipelineHandle& pipeline) override;

    private:
        const VulkanGraphicsApi& m_Api;
        onyxU8 m_FrameIndex;

        bool m_IsRecording = false;
        Reference<VulkanRenderPass> m_CurrentRenderPass;
        Reference<VulkanFramebuffer> m_CurrentFrameBuffer;

        ShaderEffectHandle m_CurrentShaderEffect;
        Reference<Pipeline> m_CurrentPipeline;

        VULKAN_HANDLE(VkCommandBuffer, CommandBuffer, nullptr);
    };
}
