#pragma once

#include <onyx/rhi/graphicsapiinterface.h>
#include <onyx/rhi/graphicsresourcepool.h>
#include <onyx/rhi/sampler.h>

namespace onyx::rhi {
class Sampler;
struct RenderPassSettings;

namespace vulkan {
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

static constexpr uint32_t BindlessTextureBinding = 0;

class VulkanGraphicsApi : public GraphicsApiInterface {
    static constexpr uint8_t CommandBufferCount = 8;

  public:
    VulkanGraphicsApi();
    ~VulkanGraphicsApi() override;

    void init( GraphicLimits& limits, const GraphicSettings& settings ) override;
    void shutdown() override;

    bool beginFrame( const FrameContext& context ) override;
    bool endFrame( const FrameContext& context ) override;

    const Instance& getInstance() const { return *m_instance; }
    const PhysicalDevice& getPhysicalDevice() const { return *m_physicalDevice; }
    const Device& getDevice() const { return *m_device; }

    SwapChain& getSwapChain() { return *m_swapChain; }
    const SwapChain& getSwapChain() const { return *m_swapChain; }

    MemoryAllocator& getAllocator() { return *m_allocator; }
    const MemoryAllocator& getAllocator() const { return *m_allocator; }

    // TODO: turn into own settings object instead of storing on device
    bool isBindless() const override { return m_isBindlessEnabled; }
    bool isDynamicRenderingEnabled() const { return m_isDynamicRenderingEnabled; }
    bool isSynchronization2Enabled() const { return m_isSynchronization2Enabled; }
    bool isTimelineSemaphoreEnabled() const { return m_isTimelineSemaphoreEnabled; }
    bool isRenderPass2ExtensionEnabled() const { return m_isRenderPass2ExtensionEnabled; }
    bool isUnifiedImageLayoutSupported() const { return m_isUnifiedImageLayoutSupported; }

    CommandBuffer& getCommandBuffer( uint8_t frameIndex ) override;
    CommandBuffer& getCommandBuffer( uint8_t frameIndex, bool shouldBegin ) override;
    CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex ) override;
    CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin ) override;

    void submitInstantCommandBuffer( Context context,
                                     uint8_t frameIndex,
                                     InplaceFunction< void( CommandBuffer& ) >&& functor ) override;

    const DescriptorPool& getDescriptorPool() const { return *m_descriptorPool; }
    Reference< rhi::Sampler > getSampler( SamplerProperties properties ) const;

    const DescriptorSetLayout& getBindlessDescriptorSetLayout() const {
        ONYX_ASSERT( m_bindlessDescriptorSetLayout != nullptr );
        return *m_bindlessDescriptorSetLayout;
    }
    const DescriptorSet& getBindlessDescriptorSet() const {
        ONYX_ASSERT( m_bindlessDescriptorSets != nullptr );
        return *m_bindlessDescriptorSets;
    }
    void releaseTexture( const VulkanTexture& texture );
    std::lock_guard< std::mutex > lockGraphicsQueue();

  private:
    void waitIdle() const override;
    void createSwapchain( const platform::Window& window ) override;

    TextureHandle& getAcquiredSwapChainImage() override;
    const TextureHandle& getAcquiredSwapChainImage() const override;
    uint32_t getAcquiredBackbufferIndex() const override;

    TextureFormat getSwapchainTextureFormat() const override;
    const Vector2s32& getSwapchainExtent() const override;

    RenderPassHandle createRenderPass( const RenderPassSettings& renderPassSettings ) override;
    FramebufferHandle createFramebuffer( const FramebufferSettings& settings ) override;

    PipelineHandle createPipeline( ShaderHandle& shader, const PipelineProperties& properties ) override;

    DynamicArray< DescriptorSetHandle > createDescriptorSet( const ShaderHandle& shader ) override;

    void createTexture( TextureHandle& outTexture,
                        const TextureStorageProperties& storageProperties,
                        const TextureProperties& properties ) override;
    void createTexture( TextureHandle& outTexture,
                        const TextureStorageProperties& storageProperties,
                        const TextureProperties& properties,
                        const Span< uint8_t >& initialData ) override;
    void createTextureView( TextureHandle& handle,
                            const Reference< VulkanTextureStorage >& textureStorage,
                            const TextureProperties& properties );
    void createAlias( TextureHandle& outTexture,
                      TextureStorageHandle& storageHandle,
                      const TextureStorageProperties& aliasStorageProperties,
                      const TextureProperties& aliasTextureProperties ) override;

    void createBuffer( BufferHandle& outBuffer, const BufferProperties& properties ) override;
    BufferHandle getTransientBuffer( uint8_t frameIndex, const BufferProperties& properties ) override;

  private:
    std::mutex m_graphicsMutex;

    UniquePtr< Instance > m_instance;
    UniquePtr< PhysicalDevice > m_physicalDevice;
    UniquePtr< Device > m_device;
    UniquePtr< DebugUtilsMessenger > m_debugUtilsMessenger;
    UniquePtr< Surface > m_surface;
    UniquePtr< SwapChain > m_swapChain;
    UniquePtr< MemoryAllocator > m_allocator;

    // can this be moved into a descriptor manager?
    UniquePtr< DescriptorPool > m_descriptorPool;
    UniquePtr< DescriptorPool > m_bindlessDescriptorPool;
    UniquePtr< DescriptorSetLayout > m_bindlessDescriptorSetLayout;
    UniquePtr< DescriptorSet > m_bindlessDescriptorSets;

    UniquePtr< CommandBufferManager > m_commandBufferManager;
    UniquePtr< CommandBufferManager > m_computeCommandBufferManager;

    // TODO: Move to seperate class?
    InplaceArray< BufferHandle, MAX_FRAMES_IN_FLIGHT > m_ringBuffer;
    uint64_t m_currentRingBufferSize = 0;

    UniquePtr< Semaphore > m_graphicsSemaphore;
    UniquePtr< Semaphore > m_computeSemaphore;

    UniquePtr< Fence > m_graphicsSingleSubmitFence;
    UniquePtr< Fence > m_computeSingleSubmitFence;

    HashMap< uint32_t, Reference< rhi::Sampler > > m_samplers;

    // TODO Move to command buffer manager
    InplaceArray< CommandBuffer*, CommandBufferCount > m_queuedCommandBuffer;
    InplaceArray< CommandBuffer*, CommandBufferCount > m_queuedComputeCommandBuffer;

    bool m_isBindlessEnabled = false;
    bool m_isDynamicRenderingEnabled = false;
    bool m_isSynchronization2Enabled = false;
    bool m_isTimelineSemaphoreEnabled = false;
    bool m_isRenderPass2ExtensionEnabled = false;
    bool m_isUnifiedImageLayoutSupported = false;

    struct TextureUpdate {
        uint32_t Index;
        VulkanTexture* Texture;
    };

    DynamicArray< TextureUpdate > m_bindlessTexturesToUpdate;
    DynamicArray< InplaceFunction< bool(), 48 > > m_deletionQueue;

    GraphicsResourcePool< VulkanTexture, 1024 > m_textures;
};
} // namespace vulkan
} // namespace onyx::rhi
