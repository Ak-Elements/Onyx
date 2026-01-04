#pragma once 

namespace Onyx::Input
{
    enum class MouseButton : onyxU16;
    enum class Key : onyxU16;
    class InputSystem;
}

struct wl_seat;
struct wl_surface;

namespace Onyx::Platform
{
    class PlatformSystem;
    class WaylandPointer;
    class WaylandKeyboard;
    class WaylandPlatformContext;

    class WaylandInput
    {
    public:
        WaylandInput(WaylandPlatformContext& platformContext, wl_seat* seat);
        ~WaylandInput();

        WaylandPlatformContext& GetContext() { ONYX_ASSERT(m_Context != nullptr); return *m_Context; }

    private:
        static void CapabilitiesCallback(void* instance, wl_seat* seat, onyxU32 capabilities);
    private:
        WaylandPlatformContext* m_Context = nullptr;
    
        wl_seat* m_Seat = nullptr;

        UniquePtr<WaylandPointer> m_Pointer;
        UniquePtr<WaylandKeyboard> m_Keyboard;

    };
}