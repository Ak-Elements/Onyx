#include <onyx/platform/linux/wayland/waylandinput.h>

#include <onyx/platform/linux/wayland/waylandpointer.h>
#include <onyx/platform/linux/wayland/waylandkeyboard.h>

#include <wayland-client.h>

namespace Onyx::Platform
{
    WaylandInput::WaylandInput(WaylandPlatformContext& context, wl_seat* seat)
        : m_Context(&context)
        , m_Seat(seat)
    {
        static const struct wl_seat_listener seat_listener = { CapabilitiesCallback };
        wl_seat_add_listener(m_Seat, &seat_listener, this);
    }

    WaylandInput::~WaylandInput() = default;

    void WaylandInput::CapabilitiesCallback(void* instance, wl_seat* seat, onyxU32 capabilities)
    {
        WaylandInput& waylandInput = *(static_cast<WaylandInput*>(instance));

        if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && (waylandInput.m_Pointer == nullptr) )
        {
            wl_pointer* pointer  = wl_seat_get_pointer(seat);
            waylandInput.m_Pointer = MakeUnique<WaylandPointer>(waylandInput, pointer);
        }
        else if (!(capabilities & WL_SEAT_CAPABILITY_POINTER) && waylandInput.m_Pointer)
        {
            waylandInput.m_Pointer.reset();
        }
        
        if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && !waylandInput.m_Keyboard)
        {
            wl_keyboard* keyboard = wl_seat_get_keyboard(seat);
            waylandInput.m_Keyboard = MakeUnique<WaylandKeyboard>(waylandInput, keyboard);
        }
        else if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && waylandInput.m_Keyboard)
        {
            waylandInput.m_Keyboard.reset();
        }
    }
}