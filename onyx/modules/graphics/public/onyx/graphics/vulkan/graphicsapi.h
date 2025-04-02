#pragma once

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/graphicsresourcepool.h>
#include <onyx/graphics/sampler.h>

namespace Onyx::Graphics
{
    class Sampler;
    struct RenderPassSettings;

    namespace Vulkan
    {
        class CommandBufferManager;
        class Semaphore;
        class Fence;
        class Pipeline;
        class DebugUtilsMessenger;
        class DescriptorPool;
        class DescriptorSetLayout;
        class DescriptorSet;
        class Device;
        class Framebuffer;
        class Instance;
        class MemoryAllocator;
        class PhysicalDevice;
        class Surface;
        class SwapChain;
        class VulkanTexture;
        class VulkanTextureStorage;

        static constexpr onyxU32 BINDLESS_TEXTURE_BINDING = 0;

        class VulkanGraphicsApi : public GraphicsApiImpl
        {
        public:
            VulkanGraphicsApi();
            ~VulkanGraphicsApi() override;

            void Init(const Window& window) override;
            void Shutdown() override;

            bool BeginFrame(const FrameContext& context) override;
            bool EndFrame(const FrameContext& context) override;

            const Instance& GetInstance() const { return *m_Instance; }
            const PhysicalDevice& GetPhysicalDevice() const { return *m_PhysicalDevice; }
            const Device& GetDevice() const { return *m_Device; }
            const Surface& GetSurface() const { return *m_Surface; }

            SwapChain& GetSwapChain() { return *m_SwapChain; }
            const SwapChain& GetSwapChain() const { return *m_SwapChain; }

            MemoryAllocator& GetAllocator() { return *m_Allocator; }
            const MemoryAllocator& GetAllocator() const { return *m_Allocator; }
            
            const Window& GetWindow() const { return *m_Window; }

            // TODO: turn into own settings object instead of storing on device
            bool IsBindless() const override { return m_IsBindlessEnabled; }
            bool IsDynamicRenderingEnabled() const { return m_IsDynamicRenderingEnabled; }
            bool IsSynchronization2Enabled() const { return m_IsSynchronization2Enabled; }
            bool IsTimelineSemaphoreEnabled() const { return m_IsTimelineSemaphoreEnabled; }
            bool IsRenderPass2ExtensionEnabled() const { return m_IsRenderPass2ExtensionEnabled; }

            CommandBuffer& GetCommandBuffer(onyxU8 frameIndex) override;
            CommandBuffer& GetCommandBuffer(onyxU8 frameIndex, bool shouldBegin) override;
            CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex) override;
            CommandBuffer& GetComputeCommandBuffer(onyxU8 frameIndex, bool shouldBegin) override;

            void SubmitInstantCommandBuffer(Context context, onyxU8 frameIndex, InplaceFunction<void(CommandBuffer&)>&& functor) override;

            const DescriptorPool& GetDescriptorPool() const { return *m_DescriptorPool; }
            Reference<Graphics::Sampler> GetSampler(SamplerProperties properties) const;

            const DescriptorSetLayout& GetBindlessDescriptorSetLayout() const { ONYX_ASSERT(m_BindlessDescriptorSetLayout != nullptr); return *m_BindlessDescriptorSetLayout; }
            const DescriptorSet& GetBindlessDescriptorSet() const { ONYX_ASSERT(m_BindlessDescriptorSets != nullptr); return *m_BindlessDescriptorSets; }
            void ReleaseTexture(const VulkanTexture& texture);
            void SignalPresent(onyxU32 presentIndex);
            std::lock_guard<std::mutex> LockGraphicsQueue();

        private:
            void WaitIdle() const override;
            void OnWindowResize(onyxU32 width, onyxU32 height) override;

            const TextureHandle& GetAcquiredSwapChainImage(onyxU8 i) const override;
            onyxU32 GetAcquiredBackbufferIndex() const override;

            TextureFormat GetSwapchainTextureFormat() const override;
            const Vector2s32& GetSwapchainExtent() const override;

            RenderPassHandle CreateRenderPass(const RenderPassSettings& renderPassSettings) override;
            FramebufferHandle CreateFramebuffer(const FramebufferSettings& settings) override;

            PipelineHandle CreatePipeline(const PipelineProperties& properties) override;
            ShaderHandle CreateShader(InplaceArray<DynamicArray<onyxU32>, MAX_SHADER_STAGES>& perStageByteCode) override;
            DynamicArray<DescriptorSetHandle> CreateDescriptorSet(const ShaderHandle& shader, const StringView& debugName) override;

            void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties) override;
            void CreateTexture(TextureHandle& outTexture, const TextureStorageProperties& storageProperties, const TextureProperties& properties, const Span<onyxU8>& initialData) override;
            void CreateTextureView(TextureHandle& handle, const Reference<VulkanTextureStorage>& textureStorage, const TextureProperties& properties);
            void CreateAlias(TextureHandle& outTexture, TextureStorageHandle& storageHandle, const TextureStorageProperties& aliasStorageProperties, const TextureProperties& aliasTextureProperties) override;
            
            void CreateBuffer(BufferHandle& outBuffer, const BufferProperties& properties) override;

        private:
            std::mutex m_GraphicsMutex;
            const Window* m_Window = nullptr;

            UniquePtr<Instance> m_Instance;
            UniquePtr<PhysicalDevice> m_PhysicalDevice;
            UniquePtr<Device> m_Device;
            UniquePtr<DebugUtilsMessenger> m_DebugUtilsMessenger;
            UniquePtr<Surface> m_Surface;
            UniquePtr<SwapChain> m_SwapChain;
            UniquePtr<MemoryAllocator> m_Allocator;

            // can this be moved into a descriptor manager?
            UniquePtr<DescriptorPool> m_DescriptorPool;
            UniquePtr<DescriptorPool> m_BindlessDescriptorPool;
            UniquePtr<DescriptorSetLayout> m_BindlessDescriptorSetLayout;
            UniquePtr<DescriptorSet> m_BindlessDescriptorSets;

            UniquePtr<CommandBufferManager> m_CommandBufferManager;
            UniquePtr<CommandBufferManager> m_ComputeCommandBufferManager;

            UniquePtr<Semaphore> m_GraphicsSemaphore;
            UniquePtr<Semaphore> m_ComputeSemaphore;
            UniquePtr<Semaphore> m_PresentSemaphore;

            UniquePtr<Fence> m_GraphicsSingleSubmitFence;
            UniquePtr<Fence> m_ComputeSingleSubmitFence;

            HashMap<onyxU32, Reference<Graphics::Sampler>> m_Samplers;

            // TODO Move to command buffer manager
            InplaceArray<CommandBuffer*, 4> m_QueuedCommandBuffer;
            InplaceArray<CommandBuffer*, 4> m_QueuedComputeCommandBuffer;

            bool m_IsBindlessEnabled = false;
            bool m_IsDynamicRenderingEnabled = false;
            bool m_IsSynchronization2Enabled = false;
            bool m_IsTimelineSemaphoreEnabled = false;
            bool m_IsRenderPass2ExtensionEnabled = false;

            struct TextureUpdate
            {
                onyxU32 Index;
                VulkanTexture* Texture;
            };

            DynamicArray<TextureUpdate> m_BindlessTexturesToUpdate;
            DynamicArray<InplaceFunction<bool(), 48>> m_DeletionQueue;

            GraphicsResourcePool<VulkanTexture, 1024> m_Textures;
        };
    }
}
