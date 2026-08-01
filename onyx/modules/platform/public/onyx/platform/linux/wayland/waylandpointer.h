#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

struct wl_pointer;
struct wl_surface;

namespace onyx::platform::wayland {
class WaylandInput;

class WaylandPointer {
  public:
    WaylandPointer( WaylandInput& input, wl_pointer* pointer );
    ~WaylandPointer();

  private:
    static void onEnterSurface( void* instance,
                                wl_pointer* pointer,
                                uint32_t serial,
                                wl_surface* surface,
                                int32_t x,
                                int32_t y );
    static void onLeaveSurface( void* instance, wl_pointer* pointer, uint32_t serial, wl_surface* surface );
    static void onMove( void* instance, wl_pointer* pointer, uint32_t time, int32_t x, int32_t y );
    static void onButton( void* instance,
                          wl_pointer* pointer,
                          uint32_t serial,
                          uint32_t time,
                          uint32_t button,
                          uint32_t state );
    static void onAxis( void* instance, wl_pointer* pointer, uint32_t time, uint32_t axis, int32_t value );

  private:
    WaylandInput* m_input = nullptr;
    wl_pointer* m_pointer = nullptr;
};
} // namespace onyx::platform::wayland

#endif
