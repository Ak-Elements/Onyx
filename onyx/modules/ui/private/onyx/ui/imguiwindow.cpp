#include <onyx/ui/imguiwindow.h>

#if ONYX_USE_IMGUI

#include <onyx/engine/enginesystem.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::ui {
void ImGuiWindow::Open() {
    if ( m_IsOpen ) {
        BringToFront();
        SetIsCollapsed( false );
        return;
    }

    m_IsOpen = true;
    OnOpen();
}

void ImGuiWindow::Close() {
    if ( m_IsOpen == false ) {
        return;
    }

    m_IsOpen = false;
    OnClose();
}

void ImGuiWindow::Render( ImGuiSystem& imguiSystem ) {
    if ( m_IsOpen == false ) {
        return;
    }

    const bool wasOpen = m_IsOpen;

    ::ImGuiWindow* window = ImGui::FindWindowByName( m_Name.c_str() );
    m_IsDocked = ( window != nullptr ) && ( window->DockId != 0 );
    OnRender( imguiSystem );

    if ( m_IsOpen && ( wasOpen == false ) ) {
        OnOpen();
    } else if ( ( m_IsOpen == false ) && wasOpen ) {
        OnClose();
    }

    OnRenderMainMenuBar();
}

void ImGuiWindow::BringToFront() {
    ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName( m_Name.c_str() );
    ::ImGui::BringWindowToDisplayFront( imguiWindow );
}

void ImGuiWindow::SetIsCollapsed( bool _isCollapsed ) {
    if ( m_IsCollapsed != _isCollapsed ) {
        m_IsCollapsed = _isCollapsed;
        ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName( m_Name.c_str() );
        ::ImGui::SetWindowCollapsed( imguiWindow, m_IsCollapsed, ImGuiCond_Always );
    }
}

bool ImGuiWindow::Begin() {
    if ( m_WindowClass != nullptr )
        ::ImGui::SetNextWindowClass( m_WindowClass );

    bool hasBegun = ::ImGui::Begin( m_Name.c_str(), &m_IsOpen, m_Flags );
    m_IsCollapsed = ::ImGui::IsWindowCollapsed();
    m_IsFocused = ::ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy );
    return hasBegun;
}

void ImGuiWindow::End() {
    ImGui::End();
}

void ImGuiWindow::SetPosition( Vector2s32 position ) {
    SetPosition( position, Vector2f32::zero() );
}

void ImGuiWindow::SetPosition( Vector2s32 position, Vector2f32 pivot ) {
    ImGui::SetNextWindowPos( { numericCast< float32 >( position.X ), numericCast< float32 >( position.Y ) },
                             ImGuiCond_None,
                             { pivot.X, pivot.Y } );
}

void ImGuiWindow::SetDefaultPosition( Vector2s32 position ) {
    SetDefaultPosition( position, Vector2f32::zero() );
}

void ImGuiWindow::SetDefaultPosition( WindowPosition position ) {
    ImGuiViewport* windowViewport = ImGui::GetWindowViewport();
    if ( windowViewport == nullptr ) {
        return;
    }

    const ImGuiStyle style = ImGui::GetStyle();
    ImVec2 mainWindowPosition = windowViewport->Pos;
    ImVec2 mainWindowSize = windowViewport->Size;

    Vector2s32 windowPosition{ numericCast< int32_t >( mainWindowPosition.x ),
                               numericCast< int32_t >( mainWindowPosition.y ) };
    Vector2f32 windowPivot;
    switch ( position ) {
    case WindowPosition::TopLeft:
        windowPivot = Vector2f32( 0.0f, 0.0f );
        windowPosition.X += style.WindowPadding.x;
        windowPosition.Y += style.WindowPadding.y;
        break;
    case WindowPosition::TopCenter:
        windowPivot = Vector2f32( 0.5f, 0.0f );
        windowPosition.X += mainWindowSize.x / 2.0f;
        windowPosition.Y += style.WindowPadding.y;
        break;
    case WindowPosition::TopRight:
        windowPivot = Vector2f32( 1.0f, 0.0f );
        windowPosition.X += mainWindowSize.x - style.WindowPadding.x;
        windowPosition.Y += style.WindowPadding.y;
        break;
    case WindowPosition::CenterLeft:
        windowPivot = Vector2f32( 0.0f, 0.5f );
        windowPosition.X += style.WindowPadding.x;
        windowPosition.Y += mainWindowSize.y / 2.0f;
        break;
    case WindowPosition::Center:
        windowPivot = Vector2f32( 0.5f, 0.5f );
        windowPosition.X += mainWindowSize.x - style.WindowPadding.x;
        windowPosition.Y += mainWindowSize.y / 2.0f;
        break;
    case WindowPosition::CenterRight:
        windowPivot = Vector2f32( 1.0f, 0.5f );
        windowPosition.X += mainWindowSize.x / 2.0f;
        windowPosition.Y += mainWindowSize.y / 2.0f;
        break;
    case WindowPosition::BottomLeft:
        windowPivot = Vector2f32( 0.0f, 1.0f );
        windowPosition.X += style.WindowPadding.x;
        windowPosition.Y += mainWindowSize.y - style.WindowPadding.y;
        break;
    case WindowPosition::BottomCenter:
        windowPivot = Vector2f32( 0.5f, 1.0f );
        windowPosition.X += mainWindowSize.x / 2.0f;
        windowPosition.Y += mainWindowSize.y - style.WindowPadding.y;
        break;
    case WindowPosition::BottomRight:
        windowPivot = Vector2f32( 1.0f, 1.0f );
        windowPosition.X += mainWindowSize.x - style.WindowPadding.x;
        windowPosition.Y += mainWindowSize.y - style.WindowPadding.y;
        break;
    }

    SetDefaultPosition( windowPosition, windowPivot );
}

void ImGuiWindow::SetDefaultPosition( Vector2s32 position, Vector2f32 pivot ) {
    ImGui::SetNextWindowPos( { numericCast< float32 >( position.X ), numericCast< float32 >( position.Y ) },
                             ImGuiCond_FirstUseEver,
                             { pivot.X, pivot.Y } );
}

bool ImGuiWindow::BeginMenuBar() {
    bool hasBegun = true;
    if ( ( m_Flags & ImGuiWindowFlags_MenuBar ) != ImGuiWindowFlags_MenuBar ) {
        hasBegun = ImGui::Begin( "MainWindow" );
    }

    return hasBegun && ImGui::BeginMenuBar();
}

void ImGuiWindow::EndMenuBar() {
    ImGui::EndMenuBar();

    if ( ( m_Flags & ImGuiWindowFlags_MenuBar ) != ImGuiWindowFlags_MenuBar ) {
        ImGui::End();
    }
}

void ImGuiWindow::OnOpen() {}

void ImGuiWindow::OnClose() {}

void ImGuiWindow::OnRenderMainMenuBar() {}
} // namespace onyx::ui
#endif