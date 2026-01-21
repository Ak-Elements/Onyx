#pragma once

#include <onyx/noncopyable.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/platform/platformfwd.h>

namespace Onyx
{
    class Engine;

    namespace Assets
    {
        class AssetSystem;
    }

    namespace Graphics
    {
        class GraphicsSystem;
   

        struct FrameContext;
        struct GraphicSettings;

        class GraphicsApiInterface : public NonCopyable
        {
            friend class GraphicsSystem;

        public:
            virtual ~GraphicsApiInterface() = default;

            virtual void Init(const GraphicSettings& settings) = 0;
            virtual void Shutdown() = 0;

            virtual bool BeginFrame(const FrameContext&) = 0;
            virtual bool EndFrame(const FrameContext&) = 0;
            
            virtual void OnWindowResize(onyxU32 width, onyxU32 height) = 0;

            virtual bool IsBindless() const = 0;

            virtual CommandBuffer& GetCommandBuffer(onyxU8 frameIndex) = 0;
            virtual CommandBuffer& GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin) = 0;
            virtual CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex) = 0;
            virtual CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin) = 0;
            virtual void SubmitInstantCommandBuffer(Context context, onyxU8 frameIndex, InplaceFunction<void(CommandBuffer&)>&& functor) = 0;

        private:
            virtual void WaitIdle() const = 0;
            virtual void CreateSwapchain(const Platform::Window& window) = 0;

            virtual TextureHandle& GetAcquiredSwapChainImage() = 0;
            virtual const TextureHandle& GetAcquiredSwapChainImage() const = 0;
            virtual onyxU32 GetAcquiredBackbufferIndex() const = 0;

            virtual TextureFormat GetSwapchainTextureFormat() const = 0;
            virtual const Vector2s32& GetSwapchainExtent() const = 0;

            virtual RenderPassHandle CreateRenderPass(const RenderPassSettings& settings) = 0;
            virtual FramebufferHandle CreateFramebuffer(const FramebufferSettings& settings) = 0;
            virtual PipelineHandle CreatePipeline(ShaderHandle& shader, const PipelineProperties& properties) = 0;
            virtual DynamicArray<DescriptorSetHandle> CreateDescriptorSet(const ShaderHandle& shader, StringView debugName) = 0;
            
            virtual void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties) = 0;
            virtual void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData) = 0;
            virtual void CreateAlias(TextureHandle& outTexture, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties) = 0;
            
            virtual void CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties) = 0;
            virtual BufferHandle GetTransientBuffer(onyxU8 frameIndex, const BufferProperties& properties) = 0;
        };
    }
}
