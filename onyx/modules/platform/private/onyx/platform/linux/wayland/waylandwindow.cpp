#include <onyx/platform/linux/wayland/waylandwindow.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/log/logger.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>

#include <wayland-client-protocol.h>
#include <wayland-xdg-decoration-client-protocol.h>
#include <wayland-xdg-shell-client-protocol.h>

///
#include <cerrno>
#include <ctime>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <utility>
///

namespace onyx::platform::wayland {
// TODO: Remove
namespace {
void randname( char* buf ) {
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    long r = ts.tv_nsec;
    for( int i = 0; i < 6; ++i ) {
        buf[ i ] = 'A' + ( r & 15 ) + ( r & 16 ) * 2;
        r >>= 5;
    }
}

int createShmFile() {
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname( name + sizeof( name ) - 7 );
        --retries;
        int fd = shm_open( name, O_RDWR | O_CREAT | O_EXCL, 0600 );
        if( fd >= 0 ) {
            shm_unlink( name );
            return fd;
        }
    } while( retries > 0 && errno == EEXIST );
    return -1;
}

int allocateShmFile( size_t size ) {
    int fd = createShmFile();
    if( fd < 0 )
        return -1;
    int ret;
    do {
        ret = ftruncate( fd, size );
    } while( ret < 0 && errno == EINTR );
    if( ret < 0 ) {
        close( fd );
        return -1;
    }
    return fd;
}
} // namespace

Window::Window( uint32_t id, WaylandPlatformContext& context, WindowSettings settings )
    : m_settings( std::move( settings ) )
    , m_id( id )
    , m_context( &context ) {
    createNativeWindow();
}

Window::~Window() = default;

void Window::show() {}

void Window::hide() {}

void Window::close() {
    setState( WindowState::Closed );
    m_closeSignal.dispatch( *this );
}

void Window::minimize() {}

void Window::maximize() {}

void Window::createNativeWindow() {
    static const struct xdg_toplevel_listener XdgToplevelListener = {
        handleToplevelConfigure,
        handleToplevelClose,
    };

    static const struct xdg_surface_listener XdgSurfaceListener = {
        handleSurfaceConfigure,
    };

    static const struct wl_surface_listener WlSurfaceListener = { handleSurfaceEnter, handleSurfaceLeave };

    m_surface = ::wl_compositor_create_surface( m_context->getCompositiorHandle() );
    wl_surface_add_listener( m_surface, &WlSurfaceListener, this );
    m_xdgSurface = ::xdg_wm_base_get_xdg_surface( m_context->getShellHandle(), m_surface );
    ::xdg_surface_add_listener( m_xdgSurface, &XdgSurfaceListener, this );

    m_xdgToplevel = ::xdg_surface_get_toplevel( m_xdgSurface );
    ::xdg_toplevel_add_listener( m_xdgToplevel, &XdgToplevelListener, this );

    ::xdg_toplevel_set_app_id( m_xdgToplevel, "onyx" );
    ::xdg_toplevel_set_title( m_xdgToplevel, m_settings.Title.c_str() );

    // if (m_settings.Mode == WindowMode::Fullscreen)
    {
        //      ::xdg_toplevel_set_fullscreen(m_xdgToplevel, NULL);
    }

    ::wl_surface_commit( m_surface );
    ::wl_display_flush( m_context->getDisplayHandle() );

    m_xdgTopLevelDecoration = ::zxdg_decoration_manager_v1_get_toplevel_decoration( m_context->getDecorationManager(),
                                                                                    m_xdgToplevel );
    zxdg_toplevel_decoration_v1_set_mode( m_xdgTopLevelDecoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE );

    wl_surface_commit( m_surface );
    wl_display_roundtrip( m_context->getDisplayHandle() );

    const int width = 1920, height = 1080;
    const int stride = width * 4;
    const int shmPoolSize = height * stride * 2;

    int fd = allocateShmFile( shmPoolSize );
    uint8_t* poolData = reinterpret_cast< uint8_t* >(
        mmap( nullptr, shmPoolSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 ) );

    // struct wl_shm *shm = ...; // Bound from registry
    struct wl_shm_pool* pool = wl_shm_create_pool( m_context->getSharedMemory(), fd, shmPoolSize );

    int index = 0;
    int offset = height * stride * index;
    struct wl_buffer* buffer = wl_shm_pool_create_buffer( pool, offset, width, height, stride, WL_SHM_FORMAT_XRGB8888 );

    uint32_t* pixels = (uint32_t*)&poolData[ offset ];
    memset( pixels, 0xcc, width * height * 4 );

    wl_surface_attach( m_surface, buffer, 0, 0 );
    wl_surface_damage( m_surface, 0, 0, UINT32_MAX, UINT32_MAX );
    wl_surface_commit( m_surface );
}

void Window::setSize( int32_t width, int32_t height ) {
    if( ( width != m_settings.Size[ 0 ] ) || ( height != m_settings.Size[ 1 ] ) ) {
        m_settings.Size[ 0 ] = width;
        m_settings.Size[ 1 ] = height;

        m_resizeSignal.dispatch( m_settings.Size );
    }
}

void Window::setMinimumSize( const Vector2s32& minSize ) {
    if( m_settings.MinSize != minSize ) {
        m_settings.MinSize = minSize;
        VectorComponentMask compared = m_settings.Size > m_settings.MinSize;
        setSize( enums::all( compared, VectorComponentMask::X ) ? m_settings.MinSize.X : m_settings.Size.X,
                 enums::all( compared, VectorComponentMask::Y ) ? m_settings.MinSize.Y : m_settings.Size.Y );
    }
}

void Window::setMaximumSize( const Vector2s32& maxSize ) {
    if( m_settings.MaxSize != maxSize ) {
        m_settings.MaxSize = maxSize;
        VectorComponentMask compared = m_settings.Size > m_settings.MaxSize;
        setSize( enums::all( compared, VectorComponentMask::X ) ? m_settings.MaxSize.X : m_settings.Size.X,
                 enums::all( compared, VectorComponentMask::Y ) ? m_settings.MaxSize.Y : m_settings.Size.Y );
    }
}

void Window::setWindowMode( WindowMode mode ) {
    // if (m_Settings.m_Mode == mode)
    //     return;

    m_settings.Mode = mode;
}

void Window::setState( WindowState state ) {
    if( m_state != state ) {
        switch( state ) {
        case WindowState::None:
        case WindowState::Hidden: {
            break;
        }
        case WindowState::Minimized: {
            minimize();
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

bool Window::getRequiredExtensions( std::vector< const char* >& outExtensions ) const {
    outExtensions.push_back( "VK_KHR_wayland_surface" );
    return true;
}

/*static*/ void Window::handleSurfaceConfigure( void* data, xdg_surface* surface, uint32_t serial ) {
    Window& windowInstance = *static_cast< Window* >( data );
    xdg_surface_ack_configure( surface, serial );

    if( windowInstance.m_isInitialized ) {
        wl_surface_commit( windowInstance.getSurfaceHandle() );
        wl_display_flush( windowInstance.getContext().getDisplayHandle() );
    }

    windowInstance.m_isInitialized = true;
    windowInstance.m_isInitialized.notify_one();
}

/*static*/ void Window::handleToplevelConfigure( void* data,
                                                 xdg_toplevel* toplevel,
                                                 int32_t width,
                                                 int32_t height,
                                                 wl_array* states ) {
    Window& windowInstance = *static_cast< Window* >( data );
    windowInstance.setSize( width, height );
}

/*static*/ void Window::handleToplevelClose( void* data, xdg_toplevel* xdgToplevel ) {
    Window& windowInstance = *static_cast< Window* >( data );
    windowInstance.close();
}

/*static*/ void Window::handleTopLevelDecorationConfigure(
    [[maybe_unused]] void* data,
    [[maybe_unused]] zxdg_toplevel_decoration_v1* zxdgToplevelDecorationV1,
    [[maybe_unused]] uint32_t mode ) {}

/*static*/ void Window::handleSurfaceEnter( [[maybe_unused]] void* data,
                                            [[maybe_unused]] wl_surface* surface,
                                            [[maybe_unused]] wl_output* output ) {}

/*static*/ void Window::handleSurfaceLeave( [[maybe_unused]] void* data,
                                            [[maybe_unused]] wl_surface* surface,
                                            [[maybe_unused]] wl_output* output ) {}

} // namespace onyx::platform::wayland
#endif
