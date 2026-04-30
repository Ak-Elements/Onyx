#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/rhi/framebuffercache.h>
#include <onyx/rhi/framecontext.h>
#include <onyx/rhi/graphiclimits.h>
#include <onyx/rhi/graphicsettings.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/rhi/presentthread.h>
#include <onyx/rhi/renderpasscache.h>
#include <onyx/rhi/shader/psocache.h>
#include <onyx/rhi/shader/shadercache.h>

#include <onyx/platform/platformfwd.h>

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::platform {
class PlatformSystem;
}

namespace onyx::rhi {
class ShaderGraphNode;
enum class ApiType : uint8_t;
enum class Context : uint8_t;

struct BufferProperties;
struct FramebufferSettings;
struct RenderPassSettings;
struct TextureProperties;
struct TextureStorageProperties;
struct ViewConstants;

class Camera;
class CommandBuffer;
class Framebuffer;
class GraphicsApiInterface;
class RenderContext;
class RenderPass;
class RenderGraph;
class ShaderCache;
class Texture;
class TextureStorage;

class GraphicsSystem : public IEngineSystem {
    friend class RenderPassCache;
    friend class FramebufferCache;
    friend class ShaderCache;

    using BeginFrameSignalT = Signal< void( const FrameContext& ) >;
    using RenderFrameSignalT = Signal< void( const FrameContext& ) >;
    using EndFrameSignalT = Signal< void( const FrameContext& ) >;

  public:
    static constexpr StringId32 TypeId{ "onyx::rhi::GraphicsSystem" };
    StringId32 getTypeId() const override { return TypeId; }

    GraphicsSystem( const GraphicSettings& settings,
                    assets::AssetSystem& assetSystem,
                    platform::PlatformSystem& platformSystem );
    ~GraphicsSystem() override;

    bool beginFrame();
    void render();
    void endFrame();

    ApiType getApiType() const { return m_settings.Api; }
    GraphicSettings& getSettings() { return m_settings; }
    const GraphicSettings& getSettings() const { return m_settings; }

    GraphicLimits& getLimits() { return m_limits; }
    const GraphicLimits& getLimits() const { return m_limits; }

    template < typename T >
    T& getApi() {
        return *static_cast< T* >( m_graphicsSystem.get() );
    }
    template < typename T >
    const T& getApi() const {
        return *static_cast< const T* >( m_graphicsSystem.get() );
    }

    uint16_t getRefreshRate() const;

    void setCamera( const Camera& camera ) { m_queuedCamera = &camera; }
    void resetCamera() { m_queuedCamera = nullptr; }

    const TextureHandle& getAcquiredSwapChainImage() const;
    TextureFormat getSwapchainTextureFormat() const;
    const Vector2s32& getSwapchainExtent() const;

    const TextureHandle& getDepthImage() const;
    TextureFormat getDepthTextureFormat() const { return m_depthTextureFormat; }
    const Vector2s32& getDepthTextureExtent() const { return m_depthTextureExtent; }

    ShaderCache& getShaderCache() { return m_shaderCache; }

    RenderPassHandle getOrCreateRenderPass( const RenderPassSettings& settings );
    FramebufferHandle getOrCreateFramebuffer( const FramebufferSettings& settings );
    ShaderInstanceHandle createShaderInstance( assets::AssetId shaderAssetId );
    ShaderInstanceHandle createShaderInstance( assets::AssetId shaderAssetId, const PipelineProperties& properties );

    void createTexture( TextureHandle& outTexture,
                        const TextureStorageProperties& storageProperties,
                        const TextureProperties& properties );
    void createTexture( TextureHandle& outTexture,
                        const TextureStorageProperties& storageProperties,
                        const TextureProperties& properties,
                        const Span< uint8_t >& initialData );
    void createAlias( TextureHandle& outTexture,
                      TextureStorageHandle& storageHandle,
                      const TextureStorageProperties& aliasStorageProperties,
                      const TextureProperties& aliasTextureProperties );

    void createBuffer( BufferHandle& outBuffer, const BufferProperties& properties );
    BufferHandle getTransientBuffer( const BufferProperties& properties );

    DynamicArray< DescriptorSetHandle > createDescriptorSet( const ShaderHandle& shader ) const;

    uint8_t getFrameIndex() const { return m_frameIndex; }
    FrameContext& getFrameContext() { return m_frameContext[ m_frameIndex ]; }
    const FrameContext& getFrameContext() const { return m_frameContext[ m_frameIndex ]; }
    const ViewConstants& getViewContsants() const { return m_frameContext[ m_frameIndex ].ViewConstants; }
    const BufferHandle& getViewConstantsBuffer() const { return m_viewConstantsUniformBuffers[ m_frameIndex ]; }

    CommandBuffer& getCommandBuffer( uint8_t frameIndex );
    CommandBuffer& getCommandBuffer( uint8_t frameIndex, bool shouldBegin );
    CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex );
    CommandBuffer& getComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin );

    void submitInstantCommandBuffer( Context context, InplaceFunction< void( CommandBuffer& ) >&& functor );
    const BlendState& getDefaultBlendState() const;

    bool isBindless() const;
#if !ONYX_IS_RETAIL
    bool isShaderDebugEnabled() const { return m_settings.IsShaderDebugEnabled; }
#endif
    void waitIdle();

    Sink< BeginFrameSignalT > onBeginFrame() { return Sink{ m_beginFrameSignal }; }
    Sink< BeginFrameSignalT > onRenderFrame() { return Sink{ m_renderFrameSignal }; }
    Sink< BeginFrameSignalT > onEndFrame() { return Sink{ m_endFrameSignal }; }

  protected:
    void loadSettings();

  private:
    void onWindowCreate( const platform::Window& window );
    void onWindowDestroy( const platform::Window& window );

    void createDepthImages( Vector2s32 extents );
    void createViewConstantBuffers();

    void onWindowResize( Vector2s32 extents );

    RenderPassHandle createRenderPass( const RenderPassSettings& settings );
    FramebufferHandle createFramebuffer( const FramebufferSettings& settings );

  private:
    std::mutex m_mutex;
    assets::AssetSystem* m_assetSystem = nullptr;
    platform::PlatformSystem* m_platformSystem = nullptr;

    GraphicLimits m_limits;
    GraphicSettings m_settings;

    PresentThread m_presentThread{ *this };

    const Camera* m_queuedCamera = nullptr;
    const Camera* m_camera = nullptr; // non owning pointer

    UniquePtr< GraphicsApiInterface > m_graphicsSystem;

    uint8_t m_frameIndex = 0;
    InplaceArray< FrameContext, MAX_FRAMES_IN_FLIGHT > m_frameContext;

    TextureFormat m_depthTextureFormat = TextureFormat::Invalid;
    Vector2s32 m_depthTextureExtent;

    InplaceArray< TextureHandle, MAX_FRAMES_IN_FLIGHT > m_depthImages;
    InplaceArray< BufferHandle, MAX_FRAMES_IN_FLIGHT > m_viewConstantsUniformBuffers;

    ShaderCache m_shaderCache{ *this };
    PsoCache m_psoCache;
    RenderPassCache m_renderPassCache{ *this };
    FramebufferCache m_framebufferCache{ *this };

    HashMap< StringId32, BlendState > m_blendStates;

    BeginFrameSignalT m_beginFrameSignal;
    RenderFrameSignalT m_renderFrameSignal;
    EndFrameSignalT m_endFrameSignal;

    bool m_hasComputeWork = false;
    bool m_hasWindowResized = false;
};
} // namespace onyx::rhi

namespace onyx {
template <>
struct Serialization< rhi::GraphicSettings > {
    static bool serialize( Serializer& serializer, const rhi::GraphicSettings& settings );
    static bool deserialize( const Deserializer& deserializer, rhi::GraphicSettings& outSettings );
};
} // namespace onyx
