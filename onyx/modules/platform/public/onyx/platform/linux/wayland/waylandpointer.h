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
    static void OnEnterSurface( void* instance,
                                wl_pointer* pointer,
                                uint32_t serial,
                                wl_surface* surface,
                                int32_t x,
                                int32_t y );
    static void OnLeaveSurface( void* instance, wl_pointer* pointer, uint32_t serial, wl_surface* surface );
    static void OnMove( void* instance, wl_pointer* pointer, uint32_t time, int32_t x, int32_t y );
    static void OnButton( void* instance,
                          wl_pointer* pointer,
                          uint32_t serial,
                          uint32_t time,
                          uint32_t button,
                          uint32_t state );
    static void OnAxis( void* instance, wl_pointer* pointer, uint32_t time, uint32_t axis, int32_t value );

  private:
    WaylandInput* m_Input = nullptr;
    wl_pointer* m_Pointer = nullptr;
};
} // namespace onyx::platform::wayland

#endif