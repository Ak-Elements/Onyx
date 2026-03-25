#include <onyx/platform/linux/wayland/waylandinput.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/platform/linux/wayland/waylandkeyboard.h>
#include <onyx/platform/linux/wayland/waylandpointer.h>

#include <wayland-client.h>

namespace onyx::platform::wayland {
WaylandInput::WaylandInput( WaylandPlatformContext& context, wl_seat* seat )
    : m_Context( &context )
    , m_Seat( seat ) {
    static const struct wl_seat_listener seat_listener = { CapabilitiesCallback };
    wl_seat_add_listener( m_Seat, &seat_listener, this );
}

WaylandInput::~WaylandInput() = default;

void WaylandInput::CapabilitiesCallback( void* instance, wl_seat* seat, uint32_t capabilities ) {
    WaylandInput& input = *( static_cast< WaylandInput* >( instance ) );

    if ( ( capabilities & WL_SEAT_CAPABILITY_POINTER ) && ( input.m_Pointer == nullptr ) ) {
        wl_pointer* pointer = wl_seat_get_pointer( seat );
        input.m_Pointer = makeUnique< WaylandPointer >( input, pointer );
    } else if ( !( capabilities & WL_SEAT_CAPABILITY_POINTER ) && input.m_Pointer ) {
        input.m_Pointer.reset();
    }

    if ( ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && !input.m_Keyboard ) {
        wl_keyboard* keyboard = wl_seat_get_keyboard( seat );
        input.m_Keyboard = makeUnique< WaylandKeyboard >( input, keyboard );
    } else if ( !( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && input.m_Keyboard ) {
        input.m_Keyboard.reset();
    }
}
} // namespace onyx::platform::wayland

#endif