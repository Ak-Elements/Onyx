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
void xdg_wm_base_ping( void* data, xdg_wm_base* shell, uint32_t serial ) {
    xdg_wm_base_pong( shell, serial );
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    xdg_wm_base_ping,
};
} // namespace

WaylandPlatformContext::WaylandPlatformContext( PlatformSystem& platformSystem )
    : m_PlatformSystem( &platformSystem ) {
    m_Display = wl_display_connect( NULL );
    ONYX_ASSERT( m_Display != nullptr, "Could not conenct to wayland display" );

    m_Registry = wl_display_get_registry( m_Display );
    ONYX_ASSERT( m_Registry != nullptr, "Could not get wayland registry" );

    static const struct wl_registry_listener wayland_registry_listener = { RegisterCallback, UnregisterCallback };
    wl_registry_add_listener( m_Registry, &wayland_registry_listener, this );
    wl_display_dispatch( m_Display );
    wl_display_roundtrip( m_Display );

    m_Xkb.Context = xkb_context_new( XKB_CONTEXT_NO_FLAGS );

    ONYX_ASSERT( m_Compositor != nullptr, "Could not bind wayland protocols. Missing Compositior" );
    ONYX_ASSERT( m_Shell != nullptr, "Could not bind wayland protocols. Missing Shell" );

    start();
}

WaylandPlatformContext::~WaylandPlatformContext() {}

void WaylandPlatformContext::RegisterCallback( void* data,
                                               wl_registry* registry,
                                               uint32_t name,
                                               const char* interface,
                                               uint32_t version ) {
    WaylandPlatformContext& platformContext = *( static_cast< WaylandPlatformContext* >( data ) );

    if ( std::strcmp( interface, wl_compositor_interface.name ) == 0 ) {
        platformContext.m_Compositor = static_cast< wl_compositor* >(
            wl_registry_bind( registry, name, &wl_compositor_interface, 3 ) );
    } else if ( std::strcmp( interface, wl_shm_interface.name ) == 0 ) {
        platformContext.m_SharedMemory = static_cast< wl_shm* >(
            wl_registry_bind( registry, name, &wl_shm_interface, 1 ) );
    } else if ( std::strcmp( interface, xdg_wm_base_interface.name ) == 0 ) {
        platformContext.m_Shell = static_cast< xdg_wm_base* >(
            wl_registry_bind( registry, name, &xdg_wm_base_interface, 1 ) );
        xdg_wm_base_add_listener( platformContext.m_Shell, &xdg_wm_base_listener, nullptr );
    } else if ( std::strcmp( interface, wl_seat_interface.name ) == 0 ) {
        wl_seat* seat = static_cast< wl_seat* >( wl_registry_bind( registry, name, &wl_seat_interface, 1 ) );
        platformContext.m_Input = MakeUnique< WaylandInput >( platformContext, seat );

    } else if ( strcmp( interface, zxdg_decoration_manager_v1_interface.name ) == 0 ) {
        platformContext.m_ZxdgDecorationManager = static_cast< zxdg_decoration_manager_v1* >(
            wl_registry_bind( registry, name, &zxdg_decoration_manager_v1_interface, 1 ) );
    } else if ( strcmp( interface, zxdg_toplevel_decoration_v1_interface.name ) == 0 ) {
        platformContext.m_ZxdgToplevelDecoration = static_cast< zxdg_toplevel_decoration_v1* >(
            wl_registry_bind( registry, name, &zxdg_toplevel_decoration_v1_interface, 1 ) );
    }
}

void WaylandPlatformContext::UnregisterCallback( void* /*data*/, wl_registry* /*registry*/, uint32_t /*name*/ ) {}

onyx::input::InputSystem& WaylandPlatformContext::GetInputSystem() {
    ONYX_ASSERT( m_PlatformSystem != nullptr );
    return m_PlatformSystem->GetInputSystem();
}

void WaylandPlatformContext::onUpdate() {
    while ( IsRunning() ) {
        while ( wl_display_prepare_read( m_Display ) != 0 ) {
            if ( wl_display_dispatch_pending( m_Display ) == -1 )
                break;
        }

        wl_display_flush( m_Display );
        wl_display_read_events( m_Display );

        if ( wl_display_dispatch_pending( m_Display ) == -1 )
            return;
    }
}

} // namespace onyx::platform::wayland

#endif