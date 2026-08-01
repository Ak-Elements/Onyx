#include <onyx/platform/linux/wayland/waylandplatformcontext.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/platform/linux/wayland/waylandinput.h>
#include <onyx/platform/platformsystem.h>

#include <wayland-client.h>
#include <wayland-xdg-decoration-client-protocol.h>
#include <wayland-xdg-shell-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

namespace onyx::platform::wayland {
namespace {
void xdgWmBasePing( void* data, xdg_wm_base* shell, uint32_t serial ) {
    xdg_wm_base_pong( shell, serial );
}

const struct xdg_wm_base_listener XdgWmBaseListener = {
    xdgWmBasePing,
};
} // namespace

WaylandPlatformContext::WaylandPlatformContext( PlatformSystem& platformSystem )
    : m_platformSystem( &platformSystem ) {
    m_display = wl_display_connect( nullptr );
    ONYX_ASSERT( m_display != nullptr, "Could not conenct to wayland display" );

    m_registry = wl_display_get_registry( m_display );
    ONYX_ASSERT( m_registry != nullptr, "Could not get wayland registry" );

    static const struct wl_registry_listener WaylandRegistryListener = { registerCallback, unregisterCallback };
    wl_registry_add_listener( m_registry, &WaylandRegistryListener, this );
    wl_display_dispatch( m_display );
    wl_display_roundtrip( m_display );

    m_xkb.Context = xkb_context_new( XKB_CONTEXT_NO_FLAGS );

    ONYX_ASSERT( m_compositor != nullptr, "Could not bind wayland protocols. Missing Compositior" );
    ONYX_ASSERT( m_shell != nullptr, "Could not bind wayland protocols. Missing Shell" );

    start();
}

WaylandPlatformContext::~WaylandPlatformContext() = default;

void WaylandPlatformContext::registerCallback( void* data,
                                               wl_registry* registry,
                                               uint32_t name,
                                               const char* interface,
                                               uint32_t version ) {
    WaylandPlatformContext& platformContext = *( static_cast< WaylandPlatformContext* >( data ) );

    if( std::strcmp( interface, wl_compositor_interface.name ) == 0 ) {
        platformContext.m_compositor = static_cast< wl_compositor* >(
            wl_registry_bind( registry, name, &wl_compositor_interface, 3 ) );
    } else if( std::strcmp( interface, wl_shm_interface.name ) == 0 ) {
        platformContext.m_sharedMemory = static_cast< wl_shm* >(
            wl_registry_bind( registry, name, &wl_shm_interface, 1 ) );
    } else if( std::strcmp( interface, xdg_wm_base_interface.name ) == 0 ) {
        platformContext.m_shell = static_cast< xdg_wm_base* >(
            wl_registry_bind( registry, name, &xdg_wm_base_interface, 1 ) );
        xdg_wm_base_add_listener( platformContext.m_shell, &XdgWmBaseListener, nullptr );
    } else if( std::strcmp( interface, wl_seat_interface.name ) == 0 ) {
        wl_seat* seat = static_cast< wl_seat* >( wl_registry_bind( registry, name, &wl_seat_interface, 1 ) );
        platformContext.m_input = makeUnique< WaylandInput >( platformContext, seat );

    } else if( strcmp( interface, zxdg_decoration_manager_v1_interface.name ) == 0 ) {
        platformContext.m_zxdgDecorationManager = static_cast< zxdg_decoration_manager_v1* >(
            wl_registry_bind( registry, name, &zxdg_decoration_manager_v1_interface, 1 ) );
    } else if( strcmp( interface, zxdg_toplevel_decoration_v1_interface.name ) == 0 ) {
        platformContext.m_zxdgToplevelDecoration = static_cast< zxdg_toplevel_decoration_v1* >(
            wl_registry_bind( registry, name, &zxdg_toplevel_decoration_v1_interface, 1 ) );
    }
}

void WaylandPlatformContext::unregisterCallback( void* /*data*/, wl_registry* /*registry*/, uint32_t /*name*/ ) {}

onyx::input::InputSystem& WaylandPlatformContext::getInputSystem() {
    ONYX_ASSERT( m_platformSystem != nullptr );
    return m_platformSystem->getInputSystem();
}

void WaylandPlatformContext::onUpdate() {
    while( isRunning() ) {
        while( wl_display_prepare_read( m_display ) != 0 ) {
            if( wl_display_dispatch_pending( m_display ) == -1 )
                break;
        }

        wl_display_flush( m_display );
        wl_display_read_events( m_display );

        if( wl_display_dispatch_pending( m_display ) == -1 )
            return;
    }
}

} // namespace onyx::platform::wayland

#endif
