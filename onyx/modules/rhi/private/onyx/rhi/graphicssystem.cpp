#include <onyx/rhi/graphicssystem.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/rhi/camera.h>
#include <onyx/rhi/graphicsapiinterface.h>
#include <onyx/rhi/vulkan/shader.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

#include <onyx/platform/platformsystem.h>
#include <onyx/platform/window.h>
#include <onyx/rhi/commandbuffer.h>

#if ONYX_USE_VULKAN
#include <onyx/rhi/vulkan/graphicsapi.h>

#include <utility>
#endif

namespace onyx {
bool Serialization< rhi::GraphicSettings >::serialize( Serializer& serializer, const rhi::GraphicSettings& settings ) {
    StringView path;
    serializer.write< "rendergraph" >( path );
    serializer.write< "api" >( settings.Api );
    serializer.write< "isbindless" >( settings.IsBindless );
    serializer.write< "isdynamicrendering" >( settings.IsDynamicRenderingEnabled );

#if !ONYX_IS_RETAIL
    serializer.write< "istimelinesamplingenabled" >( settings.IsTimeSamplingEnabled );
    serializer.write< "isdebugenabled" >( settings.IsDebugEnabled );
    serializer.write< "isshaderdebugenabled" >( settings.IsShaderDebugEnabled );
#endif
    return true;
}

bool Serialization< rhi::GraphicSettings >::deserialize( const Deserializer& deserializer,
                                                         rhi::GraphicSettings& outSettings ) {
    StringView path;
    if( deserializer.read< "rendergraph" >( path ) ) {
        outSettings.DefaultRenderGraph = assets::AssetId( FilePath( path ) );
    }

    deserializer.read< "api" >( outSettings.Api );

    deserializer.readOptional< "isbindless" >( outSettings.IsBindless );
    deserializer.readOptional< "isdynamicrendering" >( outSettings.IsDynamicRenderingEnabled );

#if !ONYX_IS_RETAIL
    deserializer.readOptional< "istimelinesamplingenabled" >( outSettings.IsTimeSamplingEnabled );
    deserializer.readOptional< "isdebugenabled" >( outSettings.IsDebugEnabled );
    deserializer.readOptional< "isshaderdebugenabled" >( outSettings.IsShaderDebugEnabled );
#endif
    return true;
}
} // namespace onyx

namespace onyx::rhi {
GraphicsSystem::GraphicsSystem( const GraphicSettings& settings,
                                assets::AssetSystem& assetSystem,
                                platform::PlatformSystem& platformSystem )
    : m_assetSystem( &assetSystem )
    , m_platformSystem( &platformSystem )
    , m_settings( settings )
    , m_depthTextureFormat( TextureFormat::DEPTH_FLOAT32 ) {
    constexpr StringId32 DefaultBlendStateId( "default" );
    constexpr StringId32 NoBlendStateId( "noblend" );
    BlendState& defaultBlendState = m_blendStates[ DefaultBlendStateId ];
    defaultBlendState.IsBlendEnabled = true;
    defaultBlendState.SourceColor = Blend::SrcAlpha;
    defaultBlendState.DestinationColor = Blend::OneMinusSrcAlpha;

    BlendState& noBlendState = m_blendStates[ NoBlendStateId ];
    noBlendState.IsBlendEnabled = false;

    for( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        m_frameContext[ i ].Api = this;
    }

    m_platformSystem->onWindowCreate< &GraphicsSystem::onWindowCreate >( this );
    m_platformSystem->onWindowDestroy< &GraphicsSystem::onWindowDestroy >( this );

    m_graphicsSystem = makeUnique< vulkan::VulkanGraphicsApi >();
    m_graphicsSystem->init( m_limits, m_settings );

    for( const UniquePtr< platform::Window >& window : m_platformSystem->GetWindows() ) {
        onWindowCreate( *window );
    }

    m_presentThread.start();
}

GraphicsSystem::~GraphicsSystem() {
    waitIdle();

    m_presentThread.Shutdown();

    m_frameContext.clear();

    m_framebufferCache.Clear();
    m_renderPassCache.Clear();

    m_psoCache.Clear();
    m_shaderCache.clear();

    m_depthImages.clear();
    m_viewConstantsUniformBuffers.clear();

    m_graphicsSystem->shutdown();

    m_platformSystem->DisconnectSignals( this );
}

void GraphicsSystem::createDepthImages( Vector2s32 extents ) {
    if( extents == m_depthTextureExtent ) {
        return;
    }

    m_depthTextureExtent = extents;

    TextureStorageProperties depthTargetStorageProperties;
    depthTargetStorageProperties.m_Size = Vector3s32{ m_depthTextureExtent, 1 };
    depthTargetStorageProperties.m_Format = m_depthTextureFormat;
    depthTargetStorageProperties.m_IsTexture = true;
    depthTargetStorageProperties.m_IsFrameBuffer = true;

    TextureProperties depthTargetViewProperties;
    depthTargetViewProperties.m_Format = depthTargetStorageProperties.m_Format;

    for( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        depthTargetStorageProperties.m_DebugName = format::format( "Depth Storage {}", i );
        depthTargetViewProperties.m_DebugName = format::format( "Depth Image {}", i );

        createTexture( m_depthImages[ i ], depthTargetStorageProperties, depthTargetViewProperties );
    }
}

void GraphicsSystem::createViewConstantBuffers() {
    BufferProperties uniformBufferProps;
    uniformBufferProps.m_Size = sizeof( ViewConstants );
    uniformBufferProps.m_UsageFlags = static_cast< uint8_t >( BufferUsage::Uniform | BufferUsage::DeviceAddress );
    uniformBufferProps.m_CpuAccess = CPUAccess::Write;

    for( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        uniformBufferProps.m_DebugName = format::format( "ViewConstants-{}", i );
        createBuffer( m_viewConstantsUniformBuffers[ i ], uniformBufferProps );
    }
}

bool GraphicsSystem::beginFrame() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;
    platform::Window& mainWindow = m_platformSystem->GetMainWindow();
    if( mainWindow.IsMinimized() )
        return false;

    if( m_camera != m_queuedCamera )
        m_camera = m_queuedCamera;

    m_hasComputeWork = false;

    FrameContext& currentFrameContext = getFrameContext();
    bool hasBegunFrame = m_graphicsSystem->beginFrame( currentFrameContext );
    if( hasBegunFrame == false ) {
        return false;
    }

    ONYX_PROFILE_MARK_FRAME_START( GPU_FRAME_NAME );

    if( m_camera != nullptr ) {
        ViewConstants& viewConstants = currentFrameContext.ViewConstants;
        viewConstants.ProjectionMatrix = m_camera->GetProjectionMatrix();
        viewConstants.InverseProjectionMatrix = m_camera->GetProjectionMatrixInverse();
        viewConstants.ViewMatrix = m_camera->GetViewMatrix();
        viewConstants.InverseViewMatrix = m_camera->GetViewMatrixInverse();
        viewConstants.ViewProjectionMatrix = m_camera->GetViewProjectionMatrix();
        viewConstants.InverseViewProjectionMatrix = m_camera->GetViewProjectionMatrixInverse();
        viewConstants.CameraPosition = Vector3f32( viewConstants.InverseViewMatrix[ 3 ] );
        viewConstants.CameraDirection = m_camera->GetDirection();
        viewConstants.Viewport = Vector2f32{ m_camera->GetViewportExtents() };
        viewConstants.Near = m_camera->GetNear();
        viewConstants.Far = m_camera->GetFar();
    }

    m_viewConstantsUniformBuffers[ m_frameIndex ].Buffer->SetData( 0,
                                                                   &currentFrameContext.ViewConstants,
                                                                   sizeof( ViewConstants ) );

    m_beginFrameSignal.Dispatch( currentFrameContext );

    return true;
}

void GraphicsSystem::render() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    m_renderFrameSignal.Dispatch( getFrameContext() );
}

void GraphicsSystem::endFrame() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    FrameContext& currentFrameContext = m_frameContext[ m_frameIndex ];

    m_endFrameSignal.Dispatch( currentFrameContext );

    // Transition image to present
    TextureHandle& swapchainTarget = m_graphicsSystem->getAcquiredSwapChainImage();
    CommandBuffer& commandBuffer = getCommandBuffer( m_frameIndex, true );

    // If not in editor
    // blit the final image on the swapchain
    //

    commandBuffer.transitionLayout( swapchainTarget, Context::Graphics, Access::None, ImageLayout::Present );

    m_graphicsSystem->endFrame( currentFrameContext );

    m_presentThread.QueuePresent( m_frameIndex, m_graphicsSystem->getAcquiredBackbufferIndex() );

    ONYX_PROFILE_MARK_FRAME_END( GPU_FRAME_NAME );

    m_frameIndex = ( m_frameIndex + 1 ) % MAX_FRAMES_IN_FLIGHT;
    FrameContext& nextFrameContext = m_frameContext[ m_frameIndex ];
    nextFrameContext.FrameIndex = m_frameIndex;
    nextFrameContext.AbsoluteFrame = currentFrameContext.AbsoluteFrame + 1;
    if( m_hasComputeWork )
        nextFrameContext.ComputeFrame = currentFrameContext.ComputeFrame + 1;
}

uint16_t GraphicsSystem::getRefreshRate() const {
    return m_settings.RefreshRate;
}

const TextureHandle& GraphicsSystem::getAcquiredSwapChainImage() const {
    return m_graphicsSystem->getAcquiredSwapChainImage();
}

TextureFormat GraphicsSystem::getSwapchainTextureFormat() const {
    return m_graphicsSystem->getSwapchainTextureFormat();
}

const Vector2s32& GraphicsSystem::getSwapchainExtent() const {
    return m_graphicsSystem->getSwapchainExtent();
}

const TextureHandle& GraphicsSystem::getDepthImage() const {
    return m_depthImages[ m_frameIndex ];
}

RenderPassHandle GraphicsSystem::getOrCreateRenderPass( const RenderPassSettings& settings ) {
    return m_renderPassCache.GetOrCreateRenderPass( settings );
}

FramebufferHandle GraphicsSystem::getOrCreateFramebuffer( const FramebufferSettings& settings ) {
    return m_framebufferCache.GetOrCreateFramebuffer( settings );
}

void GraphicsSystem::createTexture( TextureHandle& outTexture,
                                    const TextureStorageProperties& storageProperties,
                                    const TextureProperties& properties ) {
    std::lock_guard lock( m_mutex );
    m_graphicsSystem->createTexture( outTexture, storageProperties, properties );
}

void GraphicsSystem::createTexture( TextureHandle& outTexture,
                                    const TextureStorageProperties& storageProperties,
                                    const TextureProperties& properties,
                                    const Span< uint8_t >& initialData ) {
    std::lock_guard lock( m_mutex );
    if( initialData.empty() == false ) {
        const FrameContext& currentFrameContext = m_frameContext[ m_frameIndex ];
        ONYX_LOG_INFO( "Submitting instant in frame {}", currentFrameContext.AbsoluteFrame );
    }

    m_graphicsSystem->createTexture( outTexture, storageProperties, properties, initialData );
}

void GraphicsSystem::createAlias( TextureHandle& outTexture,
                                  TextureStorageHandle& storageHandle,
                                  const TextureStorageProperties& aliasStorageProperties,
                                  const TextureProperties& aliasTextureProperties ) {
    std::lock_guard lock( m_mutex );
    m_graphicsSystem->createAlias( outTexture, storageHandle, aliasStorageProperties, aliasTextureProperties );
}

void GraphicsSystem::createBuffer( BufferHandle& outBuffer, const BufferProperties& properties ) {
    return m_graphicsSystem->createBuffer( outBuffer, properties );
}

BufferHandle GraphicsSystem::getTransientBuffer( const BufferProperties& properties ) {
    return m_graphicsSystem->getTransientBuffer( m_frameIndex, properties );
}

DynamicArray< DescriptorSetHandle > GraphicsSystem::createDescriptorSet( const ShaderHandle& shader ) const {
    return m_graphicsSystem->createDescriptorSet( shader );
}

ShaderInstanceHandle GraphicsSystem::createShaderInstance( assets::AssetId shaderAssetId ) {
    PipelineProperties properties;
    return createShaderInstance( std::move( shaderAssetId ), properties );
}

ShaderInstanceHandle GraphicsSystem::createShaderInstance( assets::AssetId shaderAssetId,
                                                           const PipelineProperties& properties ) {
    ONYX_ASSERT( m_assetSystem != nullptr );

#if !ONYX_IS_RETAIL
    if( shaderAssetId.getPath().empty() ) {
        // retrieve asset name in case it's loaded with the hash only for better debugging
        const assets::AssetMetaData& meta = m_assetSystem->getAssetMeta( shaderAssetId );
        shaderAssetId = assets::AssetId( shaderAssetId.get(), meta.getName() );
    }
#endif

    ShaderHandle shader;
    m_assetSystem->getAsset( shaderAssetId, shader );

    PipelineHandle pipelineHandle = m_graphicsSystem->createPipeline( shader, properties );

    return ShaderInstanceHandle::create( *this, pipelineHandle, shader );
}

CommandBuffer& GraphicsSystem::getCommandBuffer( uint8_t frameIndex ) {
    return m_graphicsSystem->getCommandBuffer( frameIndex );
}

CommandBuffer& GraphicsSystem::getCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    return m_graphicsSystem->getCommandBuffer( frameIndex, shouldBegin );
}

CommandBuffer& GraphicsSystem::getComputeCommandBuffer( uint8_t frameIndex ) {
    m_hasComputeWork = true;
    return m_graphicsSystem->getComputeCommandBuffer( frameIndex );
}

CommandBuffer& GraphicsSystem::getComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    m_hasComputeWork = true;
    return m_graphicsSystem->getComputeCommandBuffer( frameIndex, shouldBegin );
}

void GraphicsSystem::submitInstantCommandBuffer( Context context,
                                                 InplaceFunction< void( CommandBuffer& ) >&& functor ) {
    const FrameContext& currentFrameContext = m_frameContext[ m_frameIndex ];
    ONYX_LOG_INFO( "Submitting instant in frame {}", currentFrameContext.AbsoluteFrame );
    return m_graphicsSystem->submitInstantCommandBuffer(
        context,
        m_frameIndex,
        std::forward< InplaceFunction< void( CommandBuffer& ) > >( functor ) );
}

const BlendState& GraphicsSystem::getDefaultBlendState() const {
    constexpr StringId32 DefaultBlendStateKey( "default" );
    return m_blendStates.at( DefaultBlendStateKey );
}

bool GraphicsSystem::isBindless() const {
    return m_graphicsSystem->isBindless();
}

void GraphicsSystem::waitIdle() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    m_graphicsSystem->waitIdle();
}

void GraphicsSystem::onWindowResize( Vector2s32 /*extents*/ ) {
    // if (m_Window->IsMinimized())
    //{
    //     return;
    // }

    // m_HasWindowResized = true;
    m_presentThread.ClearQueue();
    m_hasWindowResized = true;
}

void GraphicsSystem::loadSettings() {}

void GraphicsSystem::onWindowCreate( const platform::Window& window ) {
    // TODO: Add support for multiple windows

    window.OnResize().Connect< &GraphicsSystem::onWindowResize >( this );
    m_graphicsSystem->createSwapchain( window );

    // TODO: Remove depth images from graphics system -> move into render graph
    createDepthImages( window.GetFrameBufferSize() );
    createViewConstantBuffers();
}

void GraphicsSystem::onWindowDestroy( const platform::Window& /*window*/ ) {
    // TODO: For multiple windows we need to destroy the swapchain / surface here
}

RenderPassHandle GraphicsSystem::createRenderPass( const RenderPassSettings& settings ) {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    return m_graphicsSystem->createRenderPass( settings );
}

FramebufferHandle GraphicsSystem::createFramebuffer( const FramebufferSettings& settings ) {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    return m_graphicsSystem->createFramebuffer( settings );
}
} // namespace onyx::rhi
