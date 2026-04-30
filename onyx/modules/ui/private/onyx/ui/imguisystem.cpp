#include <onyx/ui/imguisystem.h>

#if ONYX_USE_IMGUI

#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>

#include <onyx/input/inputevent.h>
#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguinotify.h>
#include <onyx/ui/imguiwindow.h>
#include <onyx/ui/theme/theme.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/platform/platformsystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/propertygrid/inspectors/enginevariableinspector.h>
#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/windows/enginevariableswindow.h>

#include <ImGuizmo.h>
#include <implot.h>
#include <implot3d.h>

#if ONYX_IS_WINDOWS
#include <windows.h>
#endif

namespace onyx::ui {
ImGuiContext g_uiContext;

namespace internal {
bool g_locReloadLayout = false;
bool g_locSaveLayout = false;

ImGuiKey convertToImGuiKey( input::Key key ) {
    switch( key ) {
        using enum input::Key;
    case Tab:
        return ImGuiKey_Tab;
    case Left:
        return ImGuiKey_LeftArrow;
    case Right:
        return ImGuiKey_RightArrow;
    case Up:
        return ImGuiKey_UpArrow;
    case Down:
        return ImGuiKey_DownArrow;
    case PageUp:
        return ImGuiKey_PageUp;
    case PageDown:
        return ImGuiKey_PageDown;
    case Home:
        return ImGuiKey_Home;
    case End:
        return ImGuiKey_End;
    case Insert:
        return ImGuiKey_Insert;
    case Delete:
        return ImGuiKey_Delete;
    case Backspace:
        return ImGuiKey_Backspace;
    case Space:
        return ImGuiKey_Space;
    case Enter:
        return ImGuiKey_Enter;
    case Escape:
        return ImGuiKey_Escape;
    case Apostrophe:
        return ImGuiKey_Apostrophe;
    case Comma:
        return ImGuiKey_Comma;
    case Minus:
        return ImGuiKey_Minus;
    case Period:
        return ImGuiKey_Period;
    case Slash:
        return ImGuiKey_Slash;
    case Semicolon:
        return ImGuiKey_Semicolon;
    case Equals:
        return ImGuiKey_Equal;
    case LeftBracket:
        return ImGuiKey_LeftBracket;
    case BackSlash: // intentional fallthrough
    case NonUsBackSlash:
        return ImGuiKey_Backslash;
    case RightBracket:
        return ImGuiKey_RightBracket;
    case Grave:
        return ImGuiKey_GraveAccent;
    case CapsLock:
        return ImGuiKey_CapsLock;
    case ScrollLock:
        return ImGuiKey_ScrollLock;
    case NumPad_Lock_Clear:
        return ImGuiKey_NumLock;
    case PrintScreen:
        return ImGuiKey_PrintScreen;
    case Pause:
        return ImGuiKey_Pause;
    case NumPad_0:
        return ImGuiKey_Keypad0;
    case NumPad_1:
        return ImGuiKey_Keypad1;
    case NumPad_2:
        return ImGuiKey_Keypad2;
    case NumPad_3:
        return ImGuiKey_Keypad3;
    case NumPad_4:
        return ImGuiKey_Keypad4;
    case NumPad_5:
        return ImGuiKey_Keypad5;
    case NumPad_6:
        return ImGuiKey_Keypad6;
    case NumPad_7:
        return ImGuiKey_Keypad7;
    case NumPad_8:
        return ImGuiKey_Keypad8;
    case NumPad_9:
        return ImGuiKey_Keypad9;
    case NumPad_Period:
        return ImGuiKey_KeypadDecimal;
    case NumPad_Divide:
        return ImGuiKey_KeypadDivide;
    case NumPad_Multiply:
        return ImGuiKey_KeypadMultiply;
    case NumPad_Minus:
        return ImGuiKey_KeypadSubtract;
    case NumPad_Plus:
        return ImGuiKey_KeypadAdd;
    case NumPad_Enter:
        return ImGuiKey_KeypadEnter;
    case NumPad_Equals:
        return ImGuiKey_KeypadEqual;
    case Left_Ctrl:
        return ImGuiKey_LeftCtrl;
    case Left_Shift:
        return ImGuiKey_LeftShift;
    case Left_Alt:
        return ImGuiKey_LeftAlt;
    case Left_System:
        return ImGuiKey_LeftSuper;
    case Right_Ctrl:
        return ImGuiKey_RightCtrl;
    case Right_Shift:
        return ImGuiKey_RightShift;
    case Right_Alt:
        return ImGuiKey_RightAlt;
    case Right_System:
        return ImGuiKey_RightSuper;
    case Menu:
        return ImGuiKey_Menu;
    case Key0:
        return ImGuiKey_0;
    case Key1:
        return ImGuiKey_1;
    case Key2:
        return ImGuiKey_2;
    case Key3:
        return ImGuiKey_3;
    case Key4:
        return ImGuiKey_4;
    case Key5:
        return ImGuiKey_5;
    case Key6:
        return ImGuiKey_6;
    case Key7:
        return ImGuiKey_7;
    case Key8:
        return ImGuiKey_8;
    case Key9:
        return ImGuiKey_9;
    case A:
        return ImGuiKey_A;
    case B:
        return ImGuiKey_B;
    case C:
        return ImGuiKey_C;
    case D:
        return ImGuiKey_D;
    case E:
        return ImGuiKey_E;
    case F:
        return ImGuiKey_F;
    case G:
        return ImGuiKey_G;
    case H:
        return ImGuiKey_H;
    case I:
        return ImGuiKey_I;
    case J:
        return ImGuiKey_J;
    case K:
        return ImGuiKey_K;
    case L:
        return ImGuiKey_L;
    case M:
        return ImGuiKey_M;
    case N:
        return ImGuiKey_N;
    case O:
        return ImGuiKey_O;
    case P:
        return ImGuiKey_P;
    case Q:
        return ImGuiKey_Q;
    case R:
        return ImGuiKey_R;
    case S:
        return ImGuiKey_S;
    case T:
        return ImGuiKey_T;
    case U:
        return ImGuiKey_U;
    case V:
        return ImGuiKey_V;
    case W:
        return ImGuiKey_W;
    case X:
        return ImGuiKey_X;
    case Y:
        return ImGuiKey_Y;
    case Z:
        return ImGuiKey_Z;
    case F1:
        return ImGuiKey_F1;
    case F2:
        return ImGuiKey_F2;
    case F3:
        return ImGuiKey_F3;
    case F4:
        return ImGuiKey_F4;
    case F5:
        return ImGuiKey_F5;
    case F6:
        return ImGuiKey_F6;
    case F7:
        return ImGuiKey_F7;
    case F8:
        return ImGuiKey_F8;
    case F9:
        return ImGuiKey_F9;
    case F10:
        return ImGuiKey_F10;
    case F11:
        return ImGuiKey_F11;
    case F12:
        return ImGuiKey_F12;
    case Invalid:
    case NonUsHash:
    default:
        break;
    }

    return ImGuiKey_None;
}

} // namespace internal

ImGuiSystem::ImGuiSystem( IEngine& engine,
                          assets::AssetSystem& assetSystem,
                          input::InputSystem& inputSystem,
                          localization::LocalizationModule& localizationSystem,
                          rhi::GraphicsSystem& graphicsSystem,
                          platform::PlatformSystem& platformSystem )
    : m_engine( &engine )
    , m_platformSystem( &platformSystem )
    , m_inputSystem( &inputSystem ) {
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    assets::AssetHandle< onyx::ui::Theme > theme;
    assetSystem.getAsset( "engine:/themes/catppuccin-mocha/theme.nyx", theme );
    theme->getOnLoadedEvent().Connect< &ImGuiSystem::onThemeLoaded >( *this );

    FilePath settingsPath = file_system::path::getFullPath( "tmp:imgui.ini" );
    if( file_system::path::exists( settingsPath ) == false ) {
        settingsPath = file_system::path::getFullPath( "engine:/layouts/default.ini" );
    }

    ImGui::LoadIniSettingsFromDisk( settingsPath.string().data() );

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    constexpr StringId64 FontHash( "fonts/Roboto-Regular.ttf" );

    FilePath fontPath = file_system::path::getFullPath( "engine:/fonts/Roboto-Regular.ttf" );
    auto [ it, _ ] = m_fonts.emplace( FontHash,
                                      io.Fonts->AddFontFromFileTTF( fontPath.string().data(), 16.0f, &fontConfig ) );
    m_fonts.emplace( FontHash, io.Fonts->AddFontFromFileTTF( fontPath.string().data(), 36.0f, &fontConfig ) );

    io.FontDefault = it->second;
    initRenderBuffers( graphicsSystem );

    inputSystem.OnMouseAxisChange().Connect< &ImGuiSystem::onMouseAxisChange >( this );
    inputSystem.OnMouseButton().Connect< &ImGuiSystem::onMouseButton >( this );
    inputSystem.OnMousePositionChange().Connect< &ImGuiSystem::onMousePositionChange >( this );
    inputSystem.OnKey().Connect< &ImGuiSystem::onKey >( this );

    g_uiContext.AssetSystem = &assetSystem;
    g_uiContext.InputSystem = &inputSystem;
    g_uiContext.LocalizationSystem = &localizationSystem;

    rhi::PipelineProperties pipelineProperties;
    pipelineProperties.Shader = assets::AssetId( "engine:/shaders/imgui.oshader" );
    pipelineProperties.BlendStates.emplace( rhi::BlendState{ .SourceColor = rhi::Blend::SrcAlpha,
                                                             .DestinationColor = rhi::Blend::OneMinusSrcAlpha,
                                                             .ColorOperation = rhi::BlendOperation::Add,
                                                             .SourceAlpha = rhi::Blend::Invalid,
                                                             .DestinationAlpha = rhi::Blend::Invalid,
                                                             .AlphaOperation = rhi::BlendOperation::Add,
                                                             .IsBlendEnabled = true } );

    rhi::RenderPassSettings renderPassSettings;
    rhi::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.emplace();
    rhi::RenderPassSettings::Attachment attachment{};

    attachment.m_Format = enums::toIntegral( rhi::TextureFormat::BGRA_UNORM8 );
    attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::DontCare );
    renderPassSettings.m_Attachments.add( attachment );

    subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::RenderTarget );
    pipelineProperties.RenderPass = graphicsSystem.getOrCreateRenderPass( renderPassSettings );

    m_imguiShader = graphicsSystem.createShaderInstance( pipelineProperties.Shader, pipelineProperties );

    platform::Window& mainWindow = m_platformSystem->GetMainWindow();
    mainWindow.OnResize().Connect< &ImGuiSystem::onWindowResize >( this );

    registerWindow< EngineVariablesWindow >();
}

ImGuiSystem::~ImGuiSystem() {
    const FilePath settingsPath = file_system::path::getTempDirectory() / "imgui.ini";
    ImGui::SaveIniSettingsToDisk( settingsPath.string().data() );

    // m_Window->RemoveOnResizeHandler(this, &ImGuiSystem::OnWindowResize);
    m_inputSystem->OnMouseAxisChange().Disconnect( this );
    m_inputSystem->OnMouseButton().Disconnect( this );
    m_inputSystem->OnMousePositionChange().Disconnect( this );
    m_inputSystem->OnKey().Disconnect( this );

    ImPlot::DestroyContext();
    ImPlot3D::DestroyContext();
    ImGui::DestroyContext();
}

void ImGuiSystem::update( rhi::GraphicsSystem& graphicsSystem, DeltaGameTime deltaTime ) {
    ImGuiIO& io = ImGui::GetIO();

    g_uiContext.GraphicsSystem = &graphicsSystem;
    io.DeltaTime = std::max( numericCast< float32 >( deltaTime.DeltaMilliseconds ) * 0.001f, 0.001f );

    io.DisplaySize = ImVec2( numericCast< float32 >( m_platformSystem->GetMainWindow().GetWidth() ),
                             numericCast< float32 >( m_platformSystem->GetMainWindow().GetHeight() ) );
    io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

    //// this is an index based loop on purpose as windows might be added during rendering by other windows
    const uint32_t windowsCount = numericCast< uint32_t >( m_windows.size() );
    for( uint32_t i = 0; i < windowsCount; ++i ) {
        const UniquePtr< ImGuiWindow >& imguiWindow = m_windows[ i ];
        imguiWindow->render( *this );
    }

    ImGui::ShowDemoWindow();

    g_uiContext.GraphicsSystem = nullptr;
}

void ImGuiSystem::onBeginFrame( const rhi::FrameContext& /*frameContext*/ ) {
    ImGuiIO& io = ImGui::GetIO();
    // TODO: Fix
    auto& mainWindow = m_platformSystem->GetMainWindow();
    auto framebufferSize = mainWindow.GetFrameBufferSize();
    io.DisplaySize = ImVec2( static_cast< float32 >( framebufferSize.X ), static_cast< float32 >( framebufferSize.Y ) );

    io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

    if( internal::g_locReloadLayout ) {
        internal::g_locReloadLayout = false;
        /*FilePath settingsPath = file_system::path::GetDataDirectory() / "layouts/default_2.ini";
        ImGui::LoadIniSettingsFromDisk(settingsPath.string().data());*/
    }

    if( internal::g_locSaveLayout ) {
        internal::g_locSaveLayout = false;
        FilePath savePath;
        DynamicArray< StringView > extensions{ "ini" };
        if( onyx::file_system::FileDialog::SaveFileDialog( savePath, "Ini File", extensions ) ) {
            ImGui::SaveIniSettingsToDisk( savePath.string().data() );
        }
    }

#if ONYX_USE_SDL2
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame( m_Window.GetWindowHandle() );
#endif

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiSystem::onRenderFrame( const rhi::FrameContext& frameContext ) {
    if( m_imguiShader.isValid() == false )
        return;

    ImGui::Render();

    updateDrawBuffers( frameContext );

    rhi::CommandBuffer& commandBuffer = frameContext.Api->getCommandBuffer( frameContext.FrameIndex, true );

    const Reference< rhi::Pipeline >& pipeline = m_imguiShader->GetPipeline();
    const rhi::PipelineProperties& properties = pipeline->GetProperties();

    rhi::FramebufferSettings framebufferSettings;
    framebufferSettings.m_RenderPass = properties.RenderPass;

    const Vector2s32& swapchainExtent = frameContext.Api->getSwapchainExtent();
    framebufferSettings.m_Width = swapchainExtent.X;
    framebufferSettings.m_Height = swapchainExtent.Y;

    rhi::TextureHandle swapchainImage = frameContext.Api->getAcquiredSwapChainImage();
    framebufferSettings.m_ColorTargets.add( swapchainImage.Texture );

    rhi::FramebufferHandle frameBuffer = frameContext.Api->getOrCreateFramebuffer( framebufferSettings );

    // TODO: Do a proper barrier here for the rendergraph to be finished and the imgui pass to start
    commandBuffer.transitionLayout( swapchainImage,
                                    rhi::Context::Graphics,
                                    rhi::Access::ColorAttachmentWrite,
                                    rhi::ImageLayout::AttachmentOptimal );
    commandBuffer.globalBarrier( enums::toIntegral( rhi::Access::ColorAttachmentWrite ),
                                 0x00000400ULL,
                                 enums::toIntegral( rhi::Access::None ),
                                 0x00000001ULL );
    commandBuffer.beginRenderPass( properties.RenderPass, frameBuffer );
    commandBuffer.setViewport();
    commandBuffer.bindShaderEffect( m_imguiShader );

    ImDrawData* imDrawData = ImGui::GetDrawData();

    int32_t fbWidth = numericCast< int32_t >( imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x );
    int32_t fbHeight = numericCast< int32_t >( imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y );
    if( fbWidth <= 0 || fbHeight <= 0 )
        return;

    if( ( !imDrawData ) || ( imDrawData->CmdListsCount == 0 ) ) {
        return;
    }

    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    const uint8_t frameIndex = frameContext.FrameIndex;
    const rhi::BufferHandle& vertexBuffer = m_vertexBuffers[ frameIndex ];
    const rhi::BufferHandle& indexBuffer = m_indexBuffers[ frameIndex ];
    commandBuffer.bindVertexBuffer( vertexBuffer, 0, 0 );
    commandBuffer.bindIndexBuffer( indexBuffer, 0, rhi::IndexType::uint16 );

    struct PushConstants {
        Vector2f32 Scale;
        Vector2f32 Translate;
    } constants;

    constants.Scale = { 2.0f / imDrawData->DisplaySize.x, -2.0f / imDrawData->DisplaySize.y };
    constants.Translate = { -1.0f - imDrawData->DisplayPos.x * constants.Scale[ 0 ],
                            1.0f - imDrawData->DisplayPos.y * constants.Scale[ 1 ] };
    commandBuffer.bindPushConstants( rhi::ShaderStage::Vertex, 0, constants );

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clipOff = imDrawData->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clipScale = imDrawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    for( int32_t i = 0; i < imDrawData->CmdListsCount; i++ ) {
        const ImDrawList* cmdList = imDrawData->CmdLists[ i ];
        for( int32_t j = 0; j < cmdList->CmdBuffer.Size; j++ ) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[ j ];
            if( pcmd->UserCallback != nullptr ) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer
                // to reset render state.)
                // if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                //    commandBuffer.BindPushConstants(Graphics::ShaderStage::Vertex, 0, sizeof(PushConstants),
                //    &constants);
                // else
                pcmd->UserCallback( cmdList, pcmd );
            } else {
                ImTextureID textureId = pcmd->TextureId;

                ImVec4 clipRect;
                clipRect.x = ( pcmd->ClipRect.x - clipOff.x ) * clipScale.x;
                clipRect.y = ( pcmd->ClipRect.y - clipOff.y ) * clipScale.y;
                clipRect.z = ( pcmd->ClipRect.z - clipOff.x ) * clipScale.x;
                clipRect.w = ( pcmd->ClipRect.w - clipOff.y ) * clipScale.y;

                if( clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f ) {
                    // Negative offsets are illegal for vkCmdSetScissor
                    if( clipRect.x < 0.0f )
                        clipRect.x = 0.0f;
                    if( clipRect.y < 0.0f )
                        clipRect.y = 0.0f;

                    // Apply scissor/clipping rectangle
                    Rect2s16 scissor;
                    scissor.Position[ 0 ] = (int16_t)( clipRect.x );
                    scissor.Position[ 1 ] = (int16_t)( clipRect.y );
                    scissor.Extents[ 0 ] = (int16_t)( std::abs( clipRect.z - clipRect.x ) );
                    scissor.Extents[ 1 ] = (int16_t)( std::abs( clipRect.w - clipRect.y ) );
                    // vkCmdSetScissor(command_buffer, 0, 1, &scissor);

                    commandBuffer.setScissor( scissor );
                    commandBuffer.drawIndexed( rhi::PrimitiveTopology::Triangle,
                                               pcmd->ElemCount,
                                               1,
                                               pcmd->IdxOffset + indexOffset,
                                               pcmd->VtxOffset + vertexOffset,
                                               numericCast< uint32_t >( textureId ) );
                }
            }
        }

        vertexOffset += cmdList->VtxBuffer.Size;
        indexOffset += cmdList->IdxBuffer.Size;

        Rect2s16 scissor;
        scissor.Position = { 0, 0 };
        scissor.Extents = { (int16_t)fbWidth, (int16_t)fbHeight };
        commandBuffer.setScissor( scissor );
    }

    commandBuffer.endRenderPass();
}

void ImGuiSystem::onEndFrame( const rhi::FrameContext& /*frameContext*/ ) {
#if ONYX_IS_WINDOWS
    // if (ImGui::GetMouseCursor() != ImGuiMouseCursor_None)
    //{
    //	LPTSTR win32Cursor = IDC_ARROW;
    //
    //	switch (ImGui::GetMouseCursor())
    //	{
    //		case ImGuiMouseCursor_Arrow:        win32Cursor = IDC_ARROW; break;
    //		case ImGuiMouseCursor_TextInput:    win32Cursor = IDC_IBEAM; break;
    //		case ImGuiMouseCursor_ResizeAll:    win32Cursor = IDC_SIZEALL; break;
    //		case ImGuiMouseCursor_ResizeEW:     win32Cursor = IDC_SIZEWE; break;
    //		case ImGuiMouseCursor_ResizeNS:     win32Cursor = IDC_SIZENS; break;
    //		case ImGuiMouseCursor_ResizeNESW:   win32Cursor = IDC_SIZENESW; break;
    //		case ImGuiMouseCursor_ResizeNWSE:   win32Cursor = IDC_SIZENWSE; break;
    //		case ImGuiMouseCursor_Hand:         win32Cursor = IDC_HAND; break;
    //	}
    //
    //	m_Window->SetCursor(::LoadCursor(NULL, win32Cursor));
    // }
#endif

    ImGuiIO& io = ImGui::GetIO();

    // Update and Render additional Platform Windows
    if( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ImGui::EndFrame();
}

Optional< ImGuiWindow* > ImGuiSystem::getWindow( StringId32 windowId ) {
    auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
        return StringId32( window->getWindowId() ) == windowId;
    } );

    if( it == m_windows.end() ) {
        return {};
    }

    return it->get();
}

const ui::Theme& ImGuiSystem::getTheme() const {
    return *m_activeTheme;
}

void ImGuiSystem::initRenderBuffers( rhi::GraphicsSystem& graphicsSystem ) {
    ImGuiIO& io = ImGui::GetIO();

    //// Create font texture
    unsigned char* fontData;
    int texWidth, texHeight;

    io.Fonts->GetTexDataAsRGBA32( &fontData, &texWidth, &texHeight );
    uint32_t uploadSize = texWidth * texHeight * 4 * sizeof( char );

    //// Create target image for copy
    rhi::TextureStorageProperties storageProps;
    storageProps.m_Format = rhi::TextureFormat::RGBA_UNORM8;
    storageProps.m_Type = rhi::TextureType::Texture2D;
    storageProps.m_Size = { texWidth, texHeight, 1 };
    storageProps.m_MaxMipLevel = 1;
    storageProps.m_ArraySize = 0;
    storageProps.m_MSAAProperties = { 1, 1 }; // samples /quality
    storageProps.m_CpuAccess = rhi::CPUAccess::None;
    storageProps.m_GpuAccess = rhi::GPUAccess::Read;
    storageProps.m_IsTexture = true;
    storageProps.m_DebugName = "ImGui Font Texture Storage";

    rhi::TextureProperties textureProps;
    textureProps.m_Format = rhi::TextureFormat::RGBA_UNORM8;
    textureProps.m_AllowCubeMapLoads = false;
    textureProps.m_DebugName = "ImGui Font Texture";

    Span< uint8_t > fontTexData{ fontData, uploadSize };

    graphicsSystem.createTexture( m_fontImage, storageProps, textureProps, fontTexData );
    io.Fonts->TexID = m_fontImage.Texture->GetIndex();

    rhi::BufferProperties vertexBufferProps;
    vertexBufferProps.m_Size = 400000 * sizeof( ImDrawVert );
    vertexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Vertex );
    vertexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
    vertexBufferProps.m_DebugName = "ImGui Vertices";

    rhi::BufferProperties indexBufferProps;
    indexBufferProps.m_Size = 200000 * sizeof( ImDrawIdx );
    indexBufferProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Index );
    indexBufferProps.m_CpuAccess = rhi::CPUAccess::Write;
    indexBufferProps.m_DebugName = "ImGui Indices";

    for( uint8_t i = 0; i < rhi::MAX_FRAMES_IN_FLIGHT; ++i ) {
        rhi::BufferHandle& vertexBuffer = m_vertexBuffers[ i ];
        graphicsSystem.createBuffer( vertexBuffer, vertexBufferProps );
        m_vertexCounts.add( 400000 );

        rhi::BufferHandle& indexBuffer = m_indexBuffers[ i ];
        graphicsSystem.createBuffer( indexBuffer, indexBufferProps );
        m_indexCounts.add( 200000 );
    }
}

void ImGuiSystem::updateDrawBuffers( const rhi::FrameContext& frameContext ) {
    ImDrawData* imDrawData = ImGui::GetDrawData();

    if( ( !imDrawData ) || ( imDrawData->CmdListsCount == 0 ) ) {
        return;
    }

    int32_t fb_width = numericCast< int32_t >( imDrawData->DisplaySize.x * imDrawData->FramebufferScale.x );
    int32_t fb_height = numericCast< int32_t >( imDrawData->DisplaySize.y * imDrawData->FramebufferScale.y );
    if( fb_width <= 0 || fb_height <= 0 )
        return;

    // Note: Alignment is done inside buffer creation
    uint32_t vertexBufferSize = imDrawData->TotalVtxCount * sizeof( ImDrawVert );
    uint32_t indexBufferSize = imDrawData->TotalIdxCount * sizeof( ImDrawIdx );

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if( ( vertexBufferSize == 0 ) || ( indexBufferSize == 0 ) ) {
        return /*false*/;
    }

    // Vertex buffer
    const uint8_t frameIndex = frameContext.FrameIndex;
    rhi::BufferHandle& vertexBuffer = m_vertexBuffers[ frameIndex ];
    if( ( vertexBuffer.Buffer.isValid() == false ) || ( vertexBuffer.Buffer->IsMapped() == false ) ||
        ( m_vertexCounts[ frameIndex ] < imDrawData->TotalVtxCount ) ) {
        rhi::BufferProperties vertexBufferProps = vertexBuffer.Buffer->GetProperties();
        vertexBufferProps.m_Size = vertexBufferSize;

        frameContext.Api->createBuffer( vertexBuffer, vertexBufferProps );
        m_vertexCounts[ frameIndex ] = imDrawData->TotalVtxCount;
    }

    // Index buffer
    rhi::BufferHandle& indexBuffer = m_indexBuffers[ frameIndex ];
    if( ( indexBuffer.Buffer.isValid() == false ) || ( indexBuffer.Buffer->IsMapped() == false ) ||
        ( m_indexCounts[ frameIndex ] < imDrawData->TotalIdxCount ) ) {
        rhi::BufferProperties indexBufferProps = indexBuffer.Buffer->GetProperties();
        indexBufferProps.m_Size = indexBufferSize;

        frameContext.Api->createBuffer( indexBuffer, indexBufferProps );
        m_indexCounts[ frameIndex ] = imDrawData->TotalIdxCount;
    }

    // Upload data
    int32_t vertexCopyOffset = 0;
    int32_t indexCopyOffset = 0;

    for( int n = 0; n < imDrawData->CmdListsCount; n++ ) {
        const ImDrawList* cmd_list = imDrawData->CmdLists[ n ];
        vertexBuffer.Buffer->SetData( vertexCopyOffset,
                                      cmd_list->VtxBuffer.Data,
                                      cmd_list->VtxBuffer.Size * sizeof( ImDrawVert ) );
        indexBuffer.Buffer->SetData( indexCopyOffset,
                                     cmd_list->IdxBuffer.Data,
                                     cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx ) );

        vertexCopyOffset += cmd_list->VtxBuffer.Size * sizeof( ImDrawVert );
        indexCopyOffset += cmd_list->IdxBuffer.Size * sizeof( ImDrawIdx );
    }
}

void ImGuiSystem::onWindowResize( Vector2s32 size ) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2( numericCast< float32 >( size.X ), numericCast< float32 >( size.Y ) );
    io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );
}

void ImGuiSystem::onMouseAxisChange( const input::MouseAxisEvent& event ) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent( 0, event.Value );
}

void ImGuiSystem::onMouseButton( const input::MouseButtonEvent& event ) {
    if( static_cast< int32_t >( event.Button ) >= ImGuiMouseButton_COUNT )
        return;

    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent( static_cast< int32_t >( event.Button ) - 1, event.State == input::ButtonState::Down );
}

void ImGuiSystem::onMousePositionChange( const input::MousePositionEvent& event ) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent( numericCast< float32 >( event.Position[ 0 ] ), numericCast< float32 >( event.Position[ 1 ] ) );
}

void ImGuiSystem::onKey( const input::KeyboardEvent& keyboardEvent ) {
    ImGuiIO& io = ImGui::GetIO();

    bool isDown = keyboardEvent.State == input::ButtonState::Down;
    if( IsModifierKey( keyboardEvent.Key ) ) {
        switch( keyboardEvent.Key ) {
        case input::Key::Left_Ctrl:
        case input::Key::Right_Ctrl: {
            io.AddKeyEvent( ImGuiKey_ModCtrl, isDown );
            break;
        }
        case input::Key::Left_Shift:
        case input::Key::Right_Shift: {
            io.AddKeyEvent( ImGuiKey_ModShift, isDown );
            break;
        }
        case input::Key::Left_Alt:
        case input::Key::Right_Alt: {
            io.AddKeyEvent( ImGuiKey_ModAlt, isDown );
            break;
        }
        case input::Key::Left_System:
        case input::Key::Right_System: {
            io.AddKeyEvent( ImGuiKey_ModSuper, isDown );
            break;
        }
        default:
            ONYX_ASSERT( false, "Invalid modifier key" );
            break;
        }
    }

    io.AddKeyEvent( internal::convertToImGuiKey( keyboardEvent.Key ), isDown );

    if( isDown && ( keyboardEvent.Char != 0 ) )
        io.AddInputCharacterUTF16( keyboardEvent.Char );
}

void ImGuiSystem::onControllerAxisChange( const input::GameControllerAxisEvent& /*event*/ ) {}

void ImGuiSystem::onControllerButton( const input::GameControllerButtonEvent& /*event*/ ) {}

void ImGuiSystem::onThemeLoaded( assets::AssetHandle< Theme > loadedTheme ) {
    m_activeTheme = std::move( loadedTheme );
    m_activeTheme->apply();
}
} // namespace onyx::ui

#endif
