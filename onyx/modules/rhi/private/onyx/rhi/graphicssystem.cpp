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
    if ( deserializer.read< "rendergraph" >( path ) ) {
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
    : m_AssetSystem( &assetSystem )
    , m_PlatformSystem( &platformSystem )
    , m_Settings( settings )
    , m_DepthTextureFormat( TextureFormat::DEPTH_FLOAT32 ) {
    constexpr StringId32 defaultBlendStateId( "default" );
    constexpr StringId32 noBlendStateId( "noblend" );
    BlendState& defaultBlendState = m_BlendStates[ defaultBlendStateId ];
    defaultBlendState.IsBlendEnabled = true;
    defaultBlendState.SourceColor = Blend::SrcAlpha;
    defaultBlendState.DestinationColor = Blend::OneMinusSrcAlpha;

    BlendState& noBlendState = m_BlendStates[ noBlendStateId ];
    noBlendState.IsBlendEnabled = false;

    for ( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        m_FrameContext[ i ].Api = this;
    }

    m_PlatformSystem->onWindowCreate< &GraphicsSystem::OnWindowCreate >( this );
    m_PlatformSystem->onWindowDestroy< &GraphicsSystem::OnWindowDestroy >( this );

    m_GraphicsSystem = makeUnique< vulkan::VulkanGraphicsApi >();
    m_GraphicsSystem->Init( m_Settings );

    for ( const UniquePtr< platform::Window >& window : m_PlatformSystem->GetWindows() ) {
        OnWindowCreate( *window );
    }

    m_PresentThread.start();
}

GraphicsSystem::~GraphicsSystem() {
    WaitIdle();

    m_PresentThread.Shutdown();

    m_FrameContext.clear();

    m_FramebufferCache.Clear();
    m_RenderPassCache.Clear();

    m_PsoCache.Clear();
    m_ShaderCache.Clear();

    m_DepthImages.clear();
    m_ViewConstantsUniformBuffers.clear();

    m_GraphicsSystem->Shutdown();

    m_PlatformSystem->DisconnectSignals( this );
}

void GraphicsSystem::CreateDepthImages( Vector2s32 extents ) {
    if ( extents == m_DepthTextureExtent ) {
        return;
    }

    m_DepthTextureExtent = extents;

    TextureStorageProperties depthTargetStorageProperties;
    depthTargetStorageProperties.m_Size = Vector3s32{ m_DepthTextureExtent, 1 };
    depthTargetStorageProperties.m_Format = m_DepthTextureFormat;
    depthTargetStorageProperties.m_IsTexture = true;
    depthTargetStorageProperties.m_IsFrameBuffer = true;

    TextureProperties depthTargetViewProperties;
    depthTargetViewProperties.m_Format = depthTargetStorageProperties.m_Format;

    for ( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        depthTargetStorageProperties.m_DebugName = format::format( "Depth Storage {}", i );
        depthTargetViewProperties.m_DebugName = format::format( "Depth Image {}", i );

        CreateTexture( m_DepthImages[ i ], depthTargetStorageProperties, depthTargetViewProperties );
    }
}

void GraphicsSystem::CreateViewConstantBuffers() {
    BufferProperties uniformBufferProps;
    uniformBufferProps.m_Size = sizeof( ViewConstants );
    uniformBufferProps.m_UsageFlags = static_cast< uint8_t >( BufferUsage::Uniform | BufferUsage::DeviceAddress );
    uniformBufferProps.m_CpuAccess = CPUAccess::Write;

    for ( uint8_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        uniformBufferProps.m_DebugName = format::format( "ViewConstants-{}", i );
        CreateBuffer( m_ViewConstantsUniformBuffers[ i ], uniformBufferProps );
    }
}

bool GraphicsSystem::BeginFrame() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;
    platform::Window& mainWindow = m_PlatformSystem->GetMainWindow();
    if ( mainWindow.IsMinimized() )
        return false;

    if ( m_Camera != m_QueuedCamera )
        m_Camera = m_QueuedCamera;

    m_HasComputeWork = false;

    FrameContext& currentFrameContext = GetFrameContext();
    bool hasBegunFrame = m_GraphicsSystem->BeginFrame( currentFrameContext );
    if ( ( hasBegunFrame == false ) || m_HasWindowResized ) {
        m_HasWindowResized = false;
        m_PresentThread.ClearQueue();
        m_FramebufferCache.Clear();
        CreateDepthImages( mainWindow.GetFrameBufferSize() );
        // m_RenderGraph->OnSwapChainResized(*this);
        return false;
    }

    ONYX_PROFILE_MARK_FRAME_START( GPU_FRAME_NAME );

    if ( m_Camera != nullptr ) {
        ViewConstants& viewConstants = currentFrameContext.ViewConstants;
        viewConstants.ProjectionMatrix = m_Camera->GetProjectionMatrix();
        viewConstants.InverseProjectionMatrix = m_Camera->GetProjectionMatrixInverse();
        viewConstants.ViewMatrix = m_Camera->GetViewMatrix();
        viewConstants.InverseViewMatrix = m_Camera->GetViewMatrixInverse();
        viewConstants.ViewProjectionMatrix = m_Camera->GetViewProjectionMatrix();
        viewConstants.InverseViewProjectionMatrix = m_Camera->GetViewProjectionMatrixInverse();
        viewConstants.CameraPosition = Vector3f32( viewConstants.InverseViewMatrix[ 3 ] );
        viewConstants.CameraDirection = m_Camera->GetDirection();
        viewConstants.Viewport = Vector2f32{ m_Camera->GetViewportExtents() };
        viewConstants.Near = m_Camera->GetNear();
        viewConstants.Far = m_Camera->GetFar();
    }

    m_ViewConstantsUniformBuffers[ m_FrameIndex ].Buffer->SetData( 0,
                                                                   &currentFrameContext.ViewConstants,
                                                                   sizeof( ViewConstants ) );

    m_BeginFrameSignal.Dispatch( currentFrameContext );

    return true;
}

void GraphicsSystem::Render() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    m_RenderFrameSignal.Dispatch( GetFrameContext() );
}

void GraphicsSystem::EndFrame() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    FrameContext& currentFrameContext = m_FrameContext[ m_FrameIndex ];

    m_EndFrameSignal.Dispatch( currentFrameContext );

    // Transition image to present
    TextureHandle& swapchainTarget = m_GraphicsSystem->GetAcquiredSwapChainImage();
    CommandBuffer& commandBuffer = GetCommandBuffer( m_FrameIndex, true );

    // If not in editor
    // blit the final image on the swapchain
    //

    commandBuffer.TransitionLayout( swapchainTarget, Context::Graphics, Access::None, ImageLayout::Present );

    m_GraphicsSystem->EndFrame( currentFrameContext );

    m_PresentThread.QueuePresent( m_FrameIndex, m_GraphicsSystem->GetAcquiredBackbufferIndex() );

    ONYX_PROFILE_MARK_FRAME_END( GPU_FRAME_NAME );

    m_FrameIndex = ( m_FrameIndex + 1 ) % MAX_FRAMES_IN_FLIGHT;
    FrameContext& nextFrameContext = m_FrameContext[ m_FrameIndex ];
    nextFrameContext.FrameIndex = m_FrameIndex;
    nextFrameContext.AbsoluteFrame = currentFrameContext.AbsoluteFrame + 1;
    if ( m_HasComputeWork )
        nextFrameContext.ComputeFrame = currentFrameContext.ComputeFrame + 1;
}

uint16_t GraphicsSystem::GetRefreshRate() const {
    return m_Settings.RefreshRate;
}

const TextureHandle& GraphicsSystem::GetAcquiredSwapChainImage() const {
    return m_GraphicsSystem->GetAcquiredSwapChainImage();
}

TextureFormat GraphicsSystem::GetSwapchainTextureFormat() const {
    return m_GraphicsSystem->GetSwapchainTextureFormat();
}

const Vector2s32& GraphicsSystem::GetSwapchainExtent() const {
    return m_GraphicsSystem->GetSwapchainExtent();
}

const TextureHandle& GraphicsSystem::GetDepthImage() const {
    return m_DepthImages[ m_FrameIndex ];
}

RenderPassHandle GraphicsSystem::GetOrCreateRenderPass( const RenderPassSettings& settings ) {
    return m_RenderPassCache.GetOrCreateRenderPass( settings );
}

FramebufferHandle GraphicsSystem::GetOrCreateFramebuffer( const FramebufferSettings& settings ) {
    return m_FramebufferCache.GetOrCreateFramebuffer( settings );
}

void GraphicsSystem::CreateTexture( TextureHandle& outTexture,
                                    const TextureStorageProperties& storageProperties,
                                    const TextureProperties& properties ) {
    std::lock_guard lock( m_Mutex );
    m_GraphicsSystem->CreateTexture( outTexture, storageProperties, properties );
}

void GraphicsSystem::CreateTexture( TextureHandle& outTexture,
                                    const TextureStorageProperties& storageProperties,
                                    const TextureProperties& properties,
                                    const Span< uint8_t >& initialData ) {
    std::lock_guard lock( m_Mutex );
    if ( initialData.empty() == false ) {
        const FrameContext& currentFrameContext = m_FrameContext[ m_FrameIndex ];
        ONYX_LOG_INFO( "Submitting instant in frame {}", currentFrameContext.AbsoluteFrame );
    }

    m_GraphicsSystem->CreateTexture( outTexture, storageProperties, properties, initialData );
}

void GraphicsSystem::CreateAlias( TextureHandle& outTextrue,
                                  TextureStorageHandle& storageHandle,
                                  const TextureStorageProperties& aliasStorageProperties,
                                  const TextureProperties& aliasTextureProperties ) {
    std::lock_guard lock( m_Mutex );
    m_GraphicsSystem->CreateAlias( outTextrue, storageHandle, aliasStorageProperties, aliasTextureProperties );
}

void GraphicsSystem::CreateBuffer( BufferHandle& outBuffer, const BufferProperties& properties ) {
    return m_GraphicsSystem->CreateBuffer( outBuffer, properties );
}

BufferHandle GraphicsSystem::GetTransientBuffer( const BufferProperties& properties ) {
    return m_GraphicsSystem->GetTransientBuffer( m_FrameIndex, properties );
}

DynamicArray< DescriptorSetHandle > GraphicsSystem::CreateDescriptorSet( const ShaderHandle& shader,
                                                                         StringView debugName ) const {
    return m_GraphicsSystem->CreateDescriptorSet( shader, debugName );
}

ShaderInstanceHandle GraphicsSystem::CreateShaderInstance( assets::AssetId shaderAssetId ) {
    PipelineProperties properties;
    return CreateShaderInstance( shaderAssetId, properties );
}

ShaderInstanceHandle GraphicsSystem::CreateShaderInstance( assets::AssetId shaderAssetId,
                                                           const PipelineProperties& properties ) {
    ONYX_ASSERT( m_AssetSystem != nullptr );
    ShaderHandle shader;
    m_AssetSystem->getAsset( shaderAssetId, shader );
    PipelineHandle pipelineHandle = m_GraphicsSystem->CreatePipeline( shader, properties );

    return ShaderInstanceHandle::create( *this, pipelineHandle, shader );
}

CommandBuffer& GraphicsSystem::GetCommandBuffer( uint8_t frameIndex ) {
    return m_GraphicsSystem->GetCommandBuffer( frameIndex );
}

CommandBuffer& GraphicsSystem::GetCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    return m_GraphicsSystem->GetCommandBuffer( frameIndex, shouldBegin );
}

CommandBuffer& GraphicsSystem::GetComputeCommandBuffer( uint8_t frameIndex ) {
    m_HasComputeWork = true;
    return m_GraphicsSystem->GetComputeCommandBuffer( frameIndex );
}

CommandBuffer& GraphicsSystem::GetComputeCommandBuffer( uint8_t frameIndex, bool shouldBegin ) {
    m_HasComputeWork = true;
    return m_GraphicsSystem->GetComputeCommandBuffer( frameIndex, shouldBegin );
}

void GraphicsSystem::SubmitInstantCommandBuffer( Context context,
                                                 InplaceFunction< void( CommandBuffer& ) >&& functor ) {
    const FrameContext& currentFrameContext = m_FrameContext[ m_FrameIndex ];
    ONYX_LOG_INFO( "Submitting instant in frame {}", currentFrameContext.AbsoluteFrame );
    return m_GraphicsSystem->SubmitInstantCommandBuffer(
        context,
        m_FrameIndex,
        std::forward< InplaceFunction< void( CommandBuffer& ) > >( functor ) );
}

const BlendState& GraphicsSystem::GetDefaultBlendState() const {
    constexpr StringId32 defaultBlendStateKey( "default" );
    return m_BlendStates.at( defaultBlendStateKey );
}

bool GraphicsSystem::IsBindless() const {
    return m_GraphicsSystem->IsBindless();
}

void GraphicsSystem::WaitIdle() {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    m_GraphicsSystem->WaitIdle();
}

void GraphicsSystem::OnWindowResize( Vector2s32 /*extents*/ ) {
    // if (m_Window->IsMinimized())
    //{
    //     return;
    // }

    // m_HasWindowResized = true;
    m_PresentThread.ClearQueue();
    m_HasWindowResized = true;
}

void GraphicsSystem::LoadSettings() {}

void GraphicsSystem::OnWindowCreate( const platform::Window& window ) {
    // TODO: Add support for multiple windows

    window.OnResize().Connect< &GraphicsSystem::OnWindowResize >( this );
    m_GraphicsSystem->CreateSwapchain( window );

    // TODO: Remove depth images from graphics system -> move into render graph
    CreateDepthImages( window.GetFrameBufferSize() );
    CreateViewConstantBuffers();
}

void GraphicsSystem::OnWindowDestroy( const platform::Window& /*window*/ ) {
    // TODO: For multiple windows we need to destroy the swapchain / surface here
}

RenderPassHandle GraphicsSystem::CreateRenderPass( const RenderPassSettings& settings ) {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    return m_GraphicsSystem->CreateRenderPass( settings );
}

FramebufferHandle GraphicsSystem::CreateFramebuffer( const FramebufferSettings& settings ) {
    ONYX_PROFILE( Graphics );
    ONYX_PROFILE_FUNCTION;

    return m_GraphicsSystem->CreateFramebuffer( settings );
}
} // namespace onyx::rhi
