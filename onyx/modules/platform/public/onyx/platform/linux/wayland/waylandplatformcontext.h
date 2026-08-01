#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/thread/thread.h>

#include <onyx/platform/linux/xkb.h>

struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_seat;
struct wl_shm;

struct xdg_wm_base;

struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

namespace onyx::input {

enum class MouseButton : uint16_t;
enum class Key : uint16_t;
class InputSystem;

} // namespace onyx::input

namespace onyx::platform {

class PlatformSystem;

namespace wayland {

class WaylandInput;

class WaylandPlatformContext : public Thread {
  public:
    explicit WaylandPlatformContext( PlatformSystem& platformSystem );
    ~WaylandPlatformContext() override;

    wl_display* getDisplayHandle() { return m_display; }
    wl_registry* getRegistryHandle() { return m_registry; }
    wl_compositor* getCompositiorHandle() { return m_compositor; }
    wl_shm* getSharedMemory() { return m_sharedMemory; }

    linux::Xkb& getXkb() { return m_xkb; }

    xdg_wm_base* getShellHandle() { return m_shell; }

    zxdg_decoration_manager_v1* getDecorationManager() { return m_zxdgDecorationManager; }
    zxdg_toplevel_decoration_v1* getDecoration() { return m_zxdgToplevelDecoration; }

    onyx::input::InputSystem& getInputSystem();
    PlatformSystem& getPlatformSystem() { return *m_platformSystem; }

  private:
    static void registerCallback( void* data,
                                  wl_registry* registry,
                                  uint32_t name,
                                  const char* interface,
                                  uint32_t version );
    static void unregisterCallback( void* data, wl_registry* registry, uint32_t name );

    void onUpdate() override;

  private:
    Atomic< bool > m_isInitialized = false;
    PlatformSystem* m_platformSystem = nullptr;

    wl_display* m_display = nullptr;
    wl_registry* m_registry = nullptr;
    wl_compositor* m_compositor = nullptr;
    xdg_wm_base* m_shell = nullptr;
    wl_shm* m_sharedMemory = nullptr;

    linux::Xkb m_xkb;

    zxdg_decoration_manager_v1* m_zxdgDecorationManager = nullptr;
    zxdg_toplevel_decoration_v1* m_zxdgToplevelDecoration = nullptr;

    UniquePtr< WaylandInput > m_input;
};

} // namespace wayland

} // namespace onyx::platform

#endif
