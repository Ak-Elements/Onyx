#include <onyx/ui/imguiwindow.h>

#if ONYX_USE_IMGUI

#include <onyx/engine/enginesystem.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::ui {
void ImGuiWindow::open() {
    if( isOpen() ) {
        bringToFront();
        setIsCollapsed( false );
        return;
    }

    m_state |= ImGuiWindow::State::Opening;
}

void ImGuiWindow::close() {
    if( isOpen() == false ) {
        return;
    }

    m_state = m_state ^ ImGuiWindow::State::Open;
}

void ImGuiWindow::render( ImGuiSystem& imguiSystem ) {
    if( m_state == State::Closed ) {
        return;
    }

    ::ImGuiWindow* window = ImGui::FindWindowByName( m_name.c_str() );
    enums::set( m_state, State::Docked, ( window != nullptr ) && ( window->DockId != 0 ) );

    if( enums::all( m_state, State::Opening ) ) {
        onOpen();

        enums::set( m_state, State::Open );
        enums::unset( m_state, State::Opening );
    }

    if( isOpen() ) {
        bool isVisible = begin();

        m_dockspace.render();

        if( isVisible )
            if( beginMenuBar() ) {
                onRenderMainMenuBar();
                endMenuBar();
            }

        onRender( imguiSystem );

        end();
    }

    if( enums::all( m_state, State::Closing ) ) {
        onClose();
        m_state = State::Closed;
    }
}

void ImGuiWindow::bringToFront() {
    ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName( m_name.c_str() );
    ::ImGui::BringWindowToDisplayFront( imguiWindow );
}

void ImGuiWindow::setIsCollapsed( bool isCollapsed ) {
    if( m_isCollapsed != isCollapsed ) {
        m_isCollapsed = isCollapsed;
        ::ImGuiWindow* imguiWindow = ImGui::FindWindowByName( m_name.c_str() );
        ::ImGui::SetWindowCollapsed( imguiWindow, m_isCollapsed, ImGuiCond_Always );
    }
}

bool ImGuiWindow::begin() {
    if( m_windowClass != nullptr )
        ::ImGui::SetNextWindowClass( m_windowClass );

    if( std::numeric_limits< uint32_t >::max() != m_dockId ) {
        ImGui::SetNextWindowDockID( m_dockId );
    }

    bool open = isOpen();
    bool hasBegun = ::ImGui::Begin( m_name.c_str(), &open, m_flags );
    if( open == false ) {
        enums::set( m_state, State::Closing );
    }
    m_isCollapsed = ::ImGui::IsWindowCollapsed();
    m_isFocused = ::ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy );
    return hasBegun;
}

void ImGuiWindow::end() {
    ImGui::End();
}

void ImGuiWindow::createDockspace( uint32_t id,
                                   const ImGuiWindowClass* windowClass,
                                   const DynamicArray< DockSplit >& splits ) {
    m_dockspace.init( id, windowClass, splits );
}

void ImGuiWindow::setDefaultPosition( Vector2s32 position ) {
    setDefaultPosition( position, Vector2f32::zero() );
}

void ImGuiWindow::setDefaultPosition( Vector2s32 position, Vector2f32 pivot ) {
    ImGui::SetNextWindowPos( { numericCast< float32 >( position.X ), numericCast< float32 >( position.Y ) },
                             ImGuiCond_FirstUseEver,
                             { pivot.X, pivot.Y } );
}

void ImGuiWindow::setDefaultSize( Vector2s32 size ) {
    ImGui::SetNextWindowSize( { numericCast< float32 >( size.X ), numericCast< float32 >( size.Y ) },
                              ImGuiCond_FirstUseEver );
}

void ImGuiWindow::setDefaultPosition( WindowPosition position ) {
    ImGuiViewport* windowViewport = ImGui::GetWindowViewport();
    if( windowViewport == nullptr ) {
        return;
    }

    const ImGuiStyle style = ImGui::GetStyle();
    ImVec2 mainWindowPosition = windowViewport->Pos;
    ImVec2 mainWindowSize = windowViewport->Size;

    Vector2s32 windowPosition{ numericCast< int32_t >( mainWindowPosition.x ),
                               numericCast< int32_t >( mainWindowPosition.y ) };
    Vector2f32 windowPivot;
    switch( position ) {
    case WindowPosition::TopLeft:
        windowPivot = Vector2f32( 0.0f, 0.0f );
        windowPosition.X += numericCast< int32_t >( style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( style.WindowPadding.y );
        break;
    case WindowPosition::TopCenter:
        windowPivot = Vector2f32( 0.5f, 0.0f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x / 2.0f );
        windowPosition.Y += numericCast< int32_t >( style.WindowPadding.y );
        break;
    case WindowPosition::TopRight:
        windowPivot = Vector2f32( 1.0f, 0.0f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x - style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( style.WindowPadding.y );
        break;
    case WindowPosition::CenterLeft:
        windowPivot = Vector2f32( 0.0f, 0.5f );
        windowPosition.X += numericCast< int32_t >( style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y / 2.0f );
        break;
    case WindowPosition::Center:
        windowPivot = Vector2f32( 0.5f, 0.5f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x - style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y / 2.0f );
        break;
    case WindowPosition::CenterRight:
        windowPivot = Vector2f32( 1.0f, 0.5f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x / 2.0f );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y / 2.0f );
        break;
    case WindowPosition::BottomLeft:
        windowPivot = Vector2f32( 0.0f, 1.0f );
        windowPosition.X += numericCast< int32_t >( style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y - style.WindowPadding.y );
        break;
    case WindowPosition::BottomCenter:
        windowPivot = Vector2f32( 0.5f, 1.0f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x / 2.0f );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y - style.WindowPadding.y );
        break;
    case WindowPosition::BottomRight:
        windowPivot = Vector2f32( 1.0f, 1.0f );
        windowPosition.X += numericCast< int32_t >( mainWindowSize.x - style.WindowPadding.x );
        windowPosition.Y += numericCast< int32_t >( mainWindowSize.y - style.WindowPadding.y );
        break;
    }

    setDefaultPosition( windowPosition, windowPivot );
}

bool ImGuiWindow::beginMenuBar() const {
    bool hasBegun = true;
    if( ( m_flags & ImGuiWindowFlags_MenuBar ) != ImGuiWindowFlags_MenuBar ) {
        hasBegun = ImGui::Begin( "MainWindow" );
    }

    return hasBegun && ImGui::BeginMenuBar();
}

void ImGuiWindow::endMenuBar() const {
    ImGui::EndMenuBar();

    if( ( m_flags & ImGuiWindowFlags_MenuBar ) != ImGuiWindowFlags_MenuBar ) {
        ImGui::End();
    }
}

void ImGuiWindow::onOpen() {}
// void ImGuiWindow::onPreRender( ui::ImGuiSystem& /*system*/ ) {}
void ImGuiWindow::onClose() {}
void ImGuiWindow::onRenderMainMenuBar() {}
} // namespace onyx::ui
#endif
