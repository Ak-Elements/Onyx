#pragma once

#include <onyx/noncopyable.h>
#include <onyx/graphics/graphicshandles.h>

namespace Onyx::Graphics
{
    struct TextureHandle;
    class Buffer;
    class Pipeline;
    class Framebuffer;
    class RenderPass;

    class CommandBuffer : public NonCopyable
    {
        friend struct ConditionalRender;
    public:
        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void BeginSingleSubmit() = 0;
        virtual void End() = 0;

        virtual void BeginRenderPass(const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindShaderEffect(const ShaderInstanceHandle& shader) = 0;
        virtual void BindVertexBuffer(const BufferHandle& bufferHandle, onyxU32 binding, onyxU32 offset) = 0;
        virtual void BindVertexBuffers(const InplaceArray<BufferHandle, 8>& bufferHandles, const InplaceArray<onyxU32, 8> bufferOffsets, onyxU32 firstBinding, onyxU32 bindingCount) = 0;
        virtual void BindIndexBuffer(const BufferHandle& buffer, onyxU32 offset, IndexType indexType) = 0;

        template <typename T>
        void BindPushConstants(ShaderStage stage, const T& data)
        {
            BindPushConstants(stage, 0, sizeof(T), &data);
        }

        template <typename T>
        void BindPushConstants(ShaderStage stage, onyxU32 offset, const T& data)
        {
            BindPushConstants(stage, offset, sizeof(T), &data);
        }

        template <typename T>
        void BindPushConstants(ShaderStage stage, onyxU32 offset, const DynamicArray<T>& container)
        {
            BindPushConstants(stage, offset, static_cast<onyxU32>(container.size() * sizeof(T)), container.data());
        }

        virtual void Bind(const TextureHandle& texture, const String& bindingName) = 0;
        virtual void Bind(const BufferHandle& buffer, const String& bindingName) = 0;

        virtual void Barrier(BufferHandle& buffer, Context newContext, Access newAccess) = 0;

        virtual void SetViewport() = 0;
        virtual void SetViewport(const Viewport& viewport) = 0;
        virtual void SetScissor() = 0;
        virtual void SetScissor(Rect2s16 scissorRect) = 0;

        virtual void ClearColor(onyxF32 red, onyxF32 green, onyxF32 blue, onyxF32 alpha, onyxU32 attachmentIndex) = 0;
        virtual void ClearDepthStencil(onyxF32 depth, onyxU8 stencil) = 0;

        virtual void Draw(PrimitiveTopology topology, onyxU32 firstVertex, onyxU32 vertexCount, onyxU32 firstInstance, onyxU32 instanceCount) = 0;
        virtual void DrawIndexed(PrimitiveTopology topology, onyxU32 indexCount, onyxU32 instanceCount, onyxU32 firstIndex, onyxS32 vertexOffset, onyxU32 firstInstance) = 0;
        virtual void DrawIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) = 0;
        virtual void DrawIndirectCount(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) = 0;
        virtual void DrawIndexedIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) = 0;

        virtual void DrawMeshTask(onyxU32 groupX, onyxU32 groupY, onyxU32 groupZ) = 0;
        virtual void DrawMeshTaskIndirect(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) = 0;

        virtual void Dispatch(onyxU32 groupX, onyxU32 groupY, onyxU32 groupZ) = 0;
        virtual void DispatchIndirect(const BufferHandle& buffer) = 0;
        virtual void DispatchIndirect(const BufferHandle& buffer, onyxU32 offset) = 0;

        virtual void Copy(const BufferHandle& source, BufferHandle& destination) = 0;

        // DEBUG
        virtual void GlobalBarrier(onyxU64 srcAccess, onyxU64 dstAccess) = 0;
        
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        virtual void BeginDebugLabel(StringView label, const Vector4f32& color) = 0;
        virtual void EndDebugLabel() = 0;
#endif

    protected:
        virtual void BindPushConstants(ShaderStage stage, onyxU32 offset, onyxU32 size, const void* data) = 0;

        virtual void BeginConditionalRendering(const BufferHandle& conditionalBuffer, onyxU32 offset) = 0;
        virtual void EndConditionalRendering() = 0;
    };

    struct ConditionalRender
    {
        ConditionalRender(CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer)
            : m_CommandBuffer(&commandBuffer)
        {
            commandBuffer.BeginConditionalRendering(conditionalBuffer, 0);
        }

        ConditionalRender(CommandBuffer& commandBuffer, const BufferHandle& conditionalBuffer, onyxU32 offset)
            : m_CommandBuffer(&commandBuffer)
        {
            commandBuffer.BeginConditionalRendering(conditionalBuffer, offset);
        }

        ~ConditionalRender()
        {
            m_CommandBuffer->EndConditionalRendering();
        }

    private:
        CommandBuffer* m_CommandBuffer = nullptr;
    };
}
