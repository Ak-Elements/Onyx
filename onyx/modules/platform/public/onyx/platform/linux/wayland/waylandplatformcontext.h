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
    ~WaylandPlatformContext();

    wl_display* GetDisplayHandle() { return m_Display; }
    wl_registry* GetRegistryHandle() { return m_Registry; }
    wl_compositor* GetCompositiorHandle() { return m_Compositor; }
    wl_shm* GetSharedMemory() { return m_SharedMemory; }

    linux::Xkb& GetXkb() { return m_Xkb; }

    xdg_wm_base* GetShellHandle() { return m_Shell; }

    zxdg_decoration_manager_v1* GetDecorationManager() { return m_ZxdgDecorationManager; }
    zxdg_toplevel_decoration_v1* GetDecoration() { return m_ZxdgToplevelDecoration; }

    onyx::input::InputSystem& GetInputSystem();
    PlatformSystem& getPlatformSystem() { return *m_PlatformSystem; }

  private:
    static void RegisterCallback( void* data,
                                  wl_registry* registry,
                                  uint32_t name,
                                  const char* interface,
                                  uint32_t version );
    static void UnregisterCallback( void* data, wl_registry* registry, uint32_t name );

    void onUpdate() override;

  private:
    Atomic< bool > m_IsInitialized = false;
    PlatformSystem* m_PlatformSystem = nullptr;

    wl_display* m_Display = nullptr;
    wl_registry* m_Registry = nullptr;
    wl_compositor* m_Compositor = nullptr;
    xdg_wm_base* m_Shell = nullptr;
    wl_shm* m_SharedMemory = nullptr;

    linux::Xkb m_Xkb;

    zxdg_decoration_manager_v1* m_ZxdgDecorationManager = nullptr;
    zxdg_toplevel_decoration_v1* m_ZxdgToplevelDecoration = nullptr;

    UniquePtr< WaylandInput > m_Input;
};

} // namespace wayland

} // namespace onyx::platform

#endif