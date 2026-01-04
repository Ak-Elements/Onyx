#pragma once 

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

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

    namespace Wayland
    {
        class Pointer;
        class Keyboard;
        class PlatformContext;

        class Input
        {
        public:
            Input(PlatformContext& platformContext, wl_seat* seat);
            ~nput();

            PlatformContext& GetContext() { ONYX_ASSERT(m_Context != nullptr); return *m_Context; }

        private:
            static void CapabilitiesCallback(void* instance, wl_seat* seat, onyxU32 capabilities);
        private:
            PlatformContext* m_Context = nullptr;

            wl_seat* m_Seat = nullptr;

            UniquePtr<Pointer> m_Pointer;
            UniquePtr<Keyboard> m_Keyboard;

        };
    }
}

#endif