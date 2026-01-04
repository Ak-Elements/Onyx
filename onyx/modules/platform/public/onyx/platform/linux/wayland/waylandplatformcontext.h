#pragma once 

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/thread/thread.h>

struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_seat;
struct wl_shm;

struct xkb_context;
struct xkb_compose_state;
struct xkb_keymap;
struct xkb_state;

struct xdg_wm_base;

struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;


namespace Onyx::Input
{
    enum class MouseButton : onyxU16;
    enum class Key : onyxU16;
    class InputSystem;
}

namespace Onyx::Platform
{
    class PlatformSystem;
    class WaylandInput;

    struct Xkb
    {
        xkb_context* Context = nullptr;
        xkb_compose_state* ComposeState = nullptr;

        onyxU32 ControlIndex = 0;
        onyxU32 AltIndex = 0;
        onyxU32 ShiftIndex = 0;
        onyxU32 SuperIndex = 0;
        onyxU32 CapsLockIndex = 0;
        onyxU32 NumLockIndex = 0;

        void SetState(xkb_state *state);
        xkb_state* GetState() { return m_State; }
        const xkb_state* GetState() const { return m_State; }

        void SetKeymap(xkb_keymap *keymap);
        xkb_keymap* GetKeymap() { return m_Keymap; }
        const xkb_keymap* GetKeymap() const { return m_Keymap; }

    private:
        xkb_keymap* m_Keymap = nullptr;
        xkb_state* m_State = nullptr;
    };

    class WaylandPlatformContext : public Thread
    {
    public:
        WaylandPlatformContext(PlatformSystem& platformSystem);
        ~WaylandPlatformContext();

        wl_display* GetDisplayHandle() { return m_Display; }
        wl_registry* GetRegistryHandle() { return m_Registry; }
        wl_compositor* GetCompositiorHandle() { return m_Compositor; }
        wl_seat* GetSeatHandle() { return m_Seat; }
        wl_shm* GetSharedMemory() { return m_SharedMemory; }
        
        Xkb& GetXkb() { return m_Xkb; }

        xdg_wm_base* GetShellHandle() { return m_Shell; }

        zxdg_decoration_manager_v1* GetDecorationManager() { return m_ZxdgDecorationManager; }
        zxdg_toplevel_decoration_v1* GetDecoration() { return m_ZxdgToplevelDecoration; }
        
        Input::InputSystem& GetInputSystem();

    private:
        static void RegisterCallback(void* data, wl_registry* registry, onyxU32 name, const char* interface, onyxU32 version);
        static void UnregisterCallback(void* data, wl_registry* registry, onyxU32 name);

        void OnUpdate() override;

    private:
        Atomic<bool> m_IsInitialized = false;
        PlatformSystem* m_PlatformSystem = nullptr;

        wl_display* m_Display = nullptr;
        wl_registry* m_Registry = nullptr;
        wl_compositor* m_Compositor = nullptr;
        xdg_wm_base* m_Shell = nullptr;
        wl_seat* m_Seat = nullptr;
        wl_shm* m_SharedMemory = nullptr;

        Xkb m_Xkb;

        zxdg_decoration_manager_v1* m_ZxdgDecorationManager = nullptr;
        zxdg_toplevel_decoration_v1* m_ZxdgToplevelDecoration = nullptr;

        UniquePtr<WaylandInput> m_Input;
    };
}

#endif