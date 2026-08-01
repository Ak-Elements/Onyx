#include <onyx/platform/linux/wayland/waylandinput.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/platform/linux/wayland/waylandkeyboard.h>
#include <onyx/platform/linux/wayland/waylandpointer.h>

#include <wayland-client.h>

namespace onyx::platform::wayland {
WaylandInput::WaylandInput( WaylandPlatformContext& context, wl_seat* seat )
    : m_context( &context )
    , m_seat( seat ) {
    static const struct wl_seat_listener SeatListener = { capabilitiesCallback };
    wl_seat_add_listener( m_seat, &SeatListener, this );
}

WaylandInput::~WaylandInput() = default;

void WaylandInput::capabilitiesCallback( void* instance, wl_seat* seat, uint32_t capabilities ) {
    WaylandInput& input = *( static_cast< WaylandInput* >( instance ) );

    if( ( capabilities & WL_SEAT_CAPABILITY_POINTER ) && ( input.m_pointer == nullptr ) ) {
        wl_pointer* pointer = wl_seat_get_pointer( seat );
        input.m_pointer = makeUnique< WaylandPointer >( input, pointer );
    } else if( !( capabilities & WL_SEAT_CAPABILITY_POINTER ) && input.m_pointer ) {
        input.m_pointer.reset();
    }

    if( ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && !input.m_keyboard ) {
        wl_keyboard* keyboard = wl_seat_get_keyboard( seat );
        input.m_keyboard = makeUnique< WaylandKeyboard >( input, keyboard );
    } else if( !( capabilities & WL_SEAT_CAPABILITY_KEYBOARD ) && input.m_keyboard ) {
        input.m_keyboard.reset();
    }
}
} // namespace onyx::platform::wayland

#endif
