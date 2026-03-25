#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

namespace onyx::input {
enum class MouseButton : uint16_t;
enum class Key : uint16_t;
class InputSystem;
} // namespace onyx::input

struct wl_seat;
struct wl_surface;

namespace onyx::platform {
class PlatformSystem;

namespace wayland {
class WaylandPointer;
class WaylandKeyboard;
class WaylandPlatformContext;

class WaylandInput {
  public:
    WaylandInput( WaylandPlatformContext& platformContext, wl_seat* seat );
    ~WaylandInput();

    WaylandPlatformContext& GetContext() {
        ONYX_ASSERT( m_Context != nullptr );
        return *m_Context;
    }

  private:
    static void CapabilitiesCallback( void* instance, wl_seat* seat, uint32_t capabilities );

  private:
    WaylandPlatformContext* m_Context = nullptr;

    wl_seat* m_Seat = nullptr;

    UniquePtr< WaylandPointer > m_Pointer;
    UniquePtr< WaylandKeyboard > m_Keyboard;
};
} // namespace wayland
} // namespace onyx::platform

#endif