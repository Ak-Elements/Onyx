#pragma once

#include <onyx/noncopyable.h>
#include <onyx/platform/platformfwd.h>
#include <onyx/rhi/graphicshandles.h>

namespace onyx {
class Engine;

namespace assets {
class AssetSystem;
}

namespace rhi {
class GraphicsSystem;

struct FrameContext;
struct GraphicLimits;
struct GraphicSettings;

class GraphicsApiInterface : public NonCopyable {
    friend class GraphicsSystem;

  public:
    ~GraphicsApiInterface() override = default;

    virtual void init( GraphicLimits& limits, const GraphicSettings& settings ) = 0;
    virtual void shutdown() = 0;

    virtual bool beginFrame( const FrameContext& ) = 0;
    virtual bool endFrame( const FrameContext& ) = 0;

    ONYX_NO_DISCARD virtual bool isBindless() const = 0;

    virtual CommandBuffer& getCommandBuffer( uint8_t frameIndex ) = 0;
    virtual CommandBuffer& getCommandBuffer( uint8_t frameIndex, bool shouldBegin ) = 0;
    virtual CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex ) = 0;
    virtual CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin ) = 0;
    virtual void submitInstantCommandBuffer( Context context,
                                             uint8_t frameIndex,
                                             InplaceFunction< void( CommandBuffer& ) >&& functor ) = 0;

  private:
    virtual void waitIdle() const = 0;
    virtual void createSwapchain( const platform::Window& window ) = 0;

    virtual TextureHandle& getAcquiredSwapChainImage() = 0;
    ONYX_NO_DISCARD virtual const TextureHandle& getAcquiredSwapChainImage() const = 0;
    ONYX_NO_DISCARD virtual uint32_t getAcquiredBackbufferIndex() const = 0;

    ONYX_NO_DISCARD virtual TextureFormat getSwapchainTextureFormat() const = 0;
    ONYX_NO_DISCARD virtual const Vector2s32& getSwapchainExtent() const = 0;

    virtual RenderPassHandle createRenderPass( const RenderPassSettings& settings ) = 0;
    virtual FramebufferHandle createFramebuffer( const FramebufferSettings& settings ) = 0;
    virtual PipelineHandle createPipeline( ShaderHandle& shader, const PipelineProperties& properties ) = 0;
    virtual DynamicArray< DescriptorSetHandle > createDescriptorSet( const ShaderHandle& shader ) = 0;

    virtual void createTexture( TextureHandle& outTexture,
                                const TextureStorageProperties& storageProperties,
                                const TextureProperties& properties ) = 0;
    virtual void createTexture( TextureHandle& outTexture,
                                const TextureStorageProperties& storageProperties,
                                const TextureProperties& properties,
                                const Span< uint8_t >& initialData ) = 0;
    virtual void createAlias( TextureHandle& outTexture,
                              TextureStorageHandle& storageHandle,
                              const TextureStorageProperties& aliasStorageProperties,
                              const TextureProperties& aliasTextureProperties ) = 0;

    virtual void createBuffer( BufferHandle& outBuffer, const BufferProperties& properties ) = 0;
    virtual BufferHandle getTransientBuffer( uint8_t frameIndex, const BufferProperties& properties ) = 0;
};
} // namespace rhi
} // namespace onyx
