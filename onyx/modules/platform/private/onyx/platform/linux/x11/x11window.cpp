#include <onyx/platform/linux/x11/x11window.h>

#if ONYX_IS_LINUX && ONYX_USE_X11

#include <onyx/log/logger.h>
#include <onyx/platform/linux/x11/x11atom.h>
#include <onyx/platform/linux/x11/x11platformcontext.h>

#include <xcb/xcb.h>

namespace onyx::platform::x11 {

Window::Window( uint32_t /*id*/, X11PlatformContext& context, WindowSettings settings )
    : m_settings( std::move( settings ) )
    , m_context( &context ) {
    createWindow();
}

Window::~Window() {}

void Window::Show() {}

void Window::Hide() {}

void Window::Minimize() {}

void Window::Maximize() {}

void Window::createWindow() {
    uint32_t valueMask, valueList[ 32 ];

    xcb_connection_t* connection = m_context->getConnectionHandle();
    const xcb_screen_t& screen = m_context->getScreen();

    m_window = xcb_generate_id( connection );

    valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    valueList[ 0 ] = screen.black_pixel;
    valueList[ 1 ] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_EXPOSURE |
                     XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_PRESS |
                     XCB_EVENT_MASK_BUTTON_RELEASE;

    if ( m_settings.Mode == WindowMode::Fullscreen ) {
        m_settings.Size.X = screen.width_in_pixels;
        m_settings.Size.Y = screen.height_in_pixels;
    }

    xcb_create_window( connection,
                       XCB_COPY_FROM_PARENT,
                       m_window,
                       screen.root,
                       numericCast< int16_t >( m_settings.Position.X ),
                       numericCast< int16_t >( m_settings.Position.Y ),
                       m_settings.Size.X,
                       m_settings.Size.Y,
                       0,
                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                       screen.root_visual,
                       valueMask,
                       valueList );

    xcb_intern_atom_reply_t* reply = queryAtom( connection, true, "WM_PROTOCOLS" );

    xcb_change_property( connection,
                         XCB_PROP_MODE_REPLACE,
                         m_window,
                         ( *reply ).atom,
                         XCB_ATOM_ATOM,
                         32,
                         1,
                         &m_context->getAtomDelete().atom );

    xcb_change_property( connection,
                         XCB_PROP_MODE_REPLACE,
                         m_window,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         m_settings.Title.size(),
                         m_settings.Title.c_str() );

    free( reply );

    /**
     * Set the WM_CLASS property to display
     * title in dash tooltip and application menu
     * on GNOME and other desktop environments
     */
    String wmClass;
    wmClass = wmClass.insert( 0, m_settings.Title );
    wmClass = wmClass.insert( m_settings.Title.size(), 1, '\0' );
    xcb_change_property( connection,
                         XCB_PROP_MODE_REPLACE,
                         m_window,
                         XCB_ATOM_WM_CLASS,
                         XCB_ATOM_STRING,
                         8,
                         wmClass.size() + 2,
                         wmClass.c_str() );

    if ( m_settings.Mode == WindowMode::Fullscreen ) {
        xcb_intern_atom_reply_t* atomWmState = queryAtom( connection, false, "_NET_WM_STATE" );

        xcb_intern_atom_reply_t* atomWmFullscreen = queryAtom( connection, false, "_NET_WM_STATE_FULLSCREEN" );

        xcb_change_property( connection,
                             XCB_PROP_MODE_REPLACE,
                             m_window,
                             atomWmState->atom,
                             XCB_ATOM_ATOM,
                             32,
                             1,
                             &( atomWmFullscreen->atom ) );

        free( atomWmFullscreen );
        free( atomWmState );
    }

    xcb_map_window( connection, m_window );
}

void Window::SetSize( int32_t width, int32_t height ) {
    if ( ( width != m_settings.Size.X ) || ( height != m_settings.Size.Y ) ) {
        m_settings.Size.X = width;
        m_settings.Size.Y = height;

        uint32_t values[ 2 ]{ static_cast< uint32_t >( width ), static_cast< uint32_t >( height ) };
        xcb_configure_window( m_context->getConnectionHandle(),
                              m_window,
                              XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                              values );

        m_resizeSignal.Dispatch( m_settings.Size );
    }
}

void Window::SetMinimumSize( const Vector2s32& minSize ) {
    if ( m_settings.MinSize != minSize ) {
        m_settings.MinSize = minSize;
        VectorComponentMask compared = m_settings.Size > m_settings.MinSize;
        SetSize( enums::all( compared, VectorComponentMask::X ) ? m_settings.MinSize.X : m_settings.Size.X,
                 enums::all( compared, VectorComponentMask::Y ) ? m_settings.MinSize.Y : m_settings.Size.Y );
    }
}

void Window::SetMaximumSize( const Vector2s32& maxSize ) {
    if ( m_settings.MaxSize != maxSize ) {
        m_settings.MaxSize = maxSize;
        VectorComponentMask compared = m_settings.Size > m_settings.MaxSize;
        SetSize( enums::all( compared, VectorComponentMask::X ) ? m_settings.MaxSize.X : m_settings.Size.X,
                 enums::all( compared, VectorComponentMask::Y ) ? m_settings.MaxSize.Y : m_settings.Size.Y );
    }
}

void Window::SetWindowMode( WindowMode mode ) {
    // if (m_Settings.m_Mode == mode)
    //     return;

    m_settings.Mode = mode;
}

void Window::SetState( WindowState state ) {
    if ( m_state != state ) {
        switch ( state ) {
        case WindowState::None:
        case WindowState::Hidden: {
            break;
        }
        case WindowState::Minimized: {
            Minimize();
            break;
        }
        case WindowState::Background: {
            break;
        }
        case WindowState::Default: {
            break;
        }
        case WindowState::Maximized: {
            break;
        }
        case WindowState::Resizing: {
            break;
        }
        case WindowState::Closed: {
            break;
        }
        }

        m_state = state;
    }
}

bool Window::GetRequiredExtensions( std::vector< const char* >& outExtensions ) const {
    outExtensions.push_back( "VK_KHR_xcb_surface" );
    return true;
}
} // namespace onyx::platform::x11
#endif