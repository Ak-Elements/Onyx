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
    public:
        virtual void Reset() = 0;
        virtual void Begin() = 0;
        virtual void BeginSingleSubmit() = 0;
        virtual void End() = 0;

        virtual void BeginRenderPass(const RenderPassHandle& renderPass, const FramebufferHandle& frameBuffer) = 0;
        virtual void EndRenderPass() = 0;

        virtual void BindShaderEffect(const ShaderEffectHandle& shaderEffect) = 0;
        virtual void BindVertexBuffer(const BufferHandle& bufferHandle, onyxU32 binding, onyxU32 offset) = 0;
        virtual void BindVertexBuffers(const InplaceArray<BufferHandle, 8>& bufferHandles, const InplaceArray<onyxU32, 8> bufferOffsets, onyxU32 firstBinding, onyxU32 bindingCount) = 0;
        virtual void BindIndexBuffer(const BufferHandle& buffer, onyxU32 offset, IndexType indexType) = 0;
        virtual void BindPushConstants(ShaderStage stage, onyxU32 offset, onyxU32 size, void* data) = 0;

        virtual void Bind(const TextureHandle& texture, const String& bindingName) = 0;
        virtual void Bind(const BufferHandle& buffer, const String& bindingName) = 0;

        virtual void SetViewport() = 0;
        virtual void SetViewport(const Viewport& viewport) = 0;
        virtual void SetScissor() = 0;
        virtual void SetScissor(const Rect2Ds16& scissorRect) = 0;

        virtual void ClearColor(onyxF32 red, onyxF32 green, onyxF32 blue, onyxF32 alpha, onyxU32 attachmentIndex) = 0;
        virtual void ClearDepthStencil(onyxF32 depth, onyxU8 stencil) = 0;

        virtual void Draw(PrimitiveTopology topology, onyxU32 firstVertex, onyxU32 vertexCount, onyxU32 firstInstance, onyxU32 instanceCount) = 0;
        virtual void DrawIndexed(PrimitiveTopology topology, onyxU32 indexCount, onyxU32 instanceCount, onyxU32 firstIndex, onyxS32 vertexOffset, onyxU32 firstInstance) = 0;
        virtual void DrawIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) = 0;
        virtual void DrawIndirectCount(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) = 0;
        virtual void DrawIndexedIndirect(const BufferHandle& buffer, onyxU32 drawCount, onyxU32 offset, onyxU32 stride) = 0;

        virtual void DrawMeshTask(onyxU32 taskCount, onyxU32 firstTask) = 0;
        virtual void DrawMeshTaskIndirect(const BufferHandle& argumentBuffer, onyxU32 argumentOffset, const BufferHandle& countBuffer, onyxU32 countOffset, onyxU32 maxDraws, onyxU32 stride) = 0;

        virtual void Dispatch(onyxU32 groupX, onyxU32 groupY, onyxU32 groupZ) = 0;
        virtual void DispatchIndirect(const BufferHandle& buffer, onyxU32 offset) = 0;

        virtual void Copy(const BufferHandle& source, BufferHandle& destination) = 0;

        // DEBUG
        virtual void GlobalBarrier(uint64_t srcAccess, uint64_t dstAccess) = 0;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        virtual void BeginDebugLabel(const StringView& label, const Vector4f& color) = 0;
        virtual void EndDebugLabel() = 0;
#endif

    protected:
        virtual void BindDescriptorSets() = 0;
        virtual void BindPipeline(const PipelineHandle& pipelineHandle) = 0;
    };
}