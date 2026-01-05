#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/platform/windowsettings.h>
#include <onyx/eventhandler.h>

struct wl_surface;
struct wl_output;
struct xdg_surface;
struct xdg_toplevel;
struct wl_array;
struct zxdg_toplevel_decoration_v1;
struct zxdg_toplevel_decoration_v1;

namespace Onyx::Platform::Wayland
{
    class WaylandPlatformContext;

    class Window
    {
    public:
        Window(WaylandPlatformContext& platformContext, const WindowSettings& setting);
        ~Window();

        void Show();
        void Hide();

        void Minimize();
        void Maximize();
        void Focus() {}
        void RequestWindowAttention() {}

        void ToggleCursor(bool enable) {}
        void UpdateCursorImage() {}

        void SetTitle(StringView title) { m_Settings.Title = String(title); }
        void SetIcon(const FilePath& path) {}

        Vector2s32 GetFrameBufferSize() const { return m_Settings.Size; }
        onyxU16 GetRefreshRate() const { return m_Settings.MonitorRefreshRate; }

        void SetSize(onyxS32 width, onyxS32 height);
        void SetMinimumSize(const Vector2s32& minSize);
        void SetMaximumSize(const Vector2s32& maxSize);
        void SetWindowMode(WindowMode mode);
        void SetState(WindowState state);

        bool GetRequiredExtensions(std::vector<const char*>& outExtensions) const;

        bool IsVSyncEnabled() const { return m_Settings.UseVsync; }
        bool IsMinimized() const { return m_State == WindowState::Minimized; }

        onyxS32 GetWidth() const { return m_Settings.Size.X; }
        onyxS32 GetHeight() const { return m_Settings.Size.Y; }

        void SetCursor(void* /*cursor*/) { }

        void EnableSystemMouseCapture(bool enable) { ONYX_UNUSED(enable); }
        WaylandPlatformContext* GetContext() const { return m_Context; }
        wl_surface* GetSurfaceHandle() const { return m_Surface; }

        WindowSettings& GetSettings() { return m_Settings; }
        const WindowSettings& GetSettings() const { return m_Settings; }

        Sink<ResizeSignalT> OnResize() { return Sink(m_ResizeSignal); }
        Sink<FocusSignalT> OnFocus() { return Sink(m_FocusSignal); }
        Sink<CloseSignalT> OnClose() { return Sink(m_CloseSignal); }

    private:
        static void HandleSurfaceConfigure(void* data, xdg_surface* surface, onyxU32 serial);
        static void HandleToplevelConfigure(void* data, xdg_toplevel* toplevel, onyxS32 width, onyxS32 height, wl_array* states);
        static void HandleToplevelClose(void* data, xdg_toplevel* xdg_toplevel);
        static void HandleTopLevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1, onyxU32 mode);

        static void HandleSurfaceEnter(void* data, wl_surface* surface, wl_output* output);
        static void HandleSurfaceLeave(void* data, wl_surface* surface, wl_output* output);

        void CreateNativeWindow();
        void FitToMonitor() {}
        void CaptureCursor() {}
        void ReleaseCursor() {}

        onyxS32 GetStyle() { return 0; }
        onyxS32 GetExtendedStyle() { return 0; }

    private:
        WindowSettings m_Settings;

        ResizeSignalT m_ResizeSignal;
        FocusSignalT m_FocusSignal;
        CloseSignalT m_CloseSignal;

        Atomic<bool> m_IsInitialized = false;
        WindowState m_State = WindowState::None;
    
        WaylandPlatformContext* m_Context = nullptr;

        wl_surface* m_Surface = nullptr;
        xdg_surface* m_XdgSurface = nullptr;;
        xdg_toplevel* m_XdgToplevel = nullptr;
        zxdg_toplevel_decoration_v1* m_XdgTopLevelDecoration = nullptr;
    };
}
#endif