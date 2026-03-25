#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

struct wl_pointer;
struct wl_surface;
struct wl_keyboard;
struct wl_array;

namespace onyx::platform::wayland {
class WaylandInput;

class WaylandKeyboard {
  public:
    WaylandKeyboard( WaylandInput& input, wl_keyboard* keyboard );
    ~WaylandKeyboard();

  private:
    static void OnKeyMap( void* inputInstance, wl_keyboard* keyboard, uint32_t format, int32_t fd, uint32_t size );
    static void OnEnterSurface( void* inputInstance,
                                wl_keyboard* keyboard,
                                uint32_t serial,
                                wl_surface* surface,
                                wl_array* keys );
    static void OnLeaveSurface( void* inputInstance, wl_keyboard* keyboard, uint32_t serial, wl_surface* surface );
    static void OnKeyChange( void* inputInstance,
                             wl_keyboard* keyboard,
                             uint32_t serial,
                             uint32_t time,
                             uint32_t key,
                             uint32_t state );
    static void OnModifierChange( void* inputInstance,
                                  wl_keyboard* keyboard,
                                  uint32_t serial,
                                  uint32_t modsDepressed,
                                  uint32_t modsLatched,
                                  uint32_t modsLocked,
                                  uint32_t group );

  private:
    WaylandInput* m_Input = nullptr;
    wl_keyboard* m_Keyboard = nullptr;
};
} // namespace onyx::platform::wayland

#endif