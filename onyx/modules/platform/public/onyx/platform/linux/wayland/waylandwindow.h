#pragma once

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/eventhandler.h>
#include <onyx/platform/windowsettings.h>

struct wl_surface;
struct wl_output;
struct xdg_surface;
struct xdg_toplevel;
struct wl_array;
struct zxdg_toplevel_decoration_v1;
struct zxdg_toplevel_decoration_v1;

namespace onyx::platform::wayland {
class WaylandPlatformContext;

class Window {
  public:
    Window( uint32_t id, WaylandPlatformContext& platformContext, const WindowSettings& setting );
    ~Window();

    void Show();
    void Hide();

    void close();

    void Minimize();
    void Maximize();
    void Focus() {}
    void RequestWindowAttention() {}

    void ToggleCursor( bool enable ) {}
    void UpdateCursorImage() {}

    void SetTitle( StringView title ) { m_Settings.Title = String( title ); }
    void SetIcon( const FilePath& path ) {}

    Vector2s32 GetFrameBufferSize() const { return m_Settings.Size; }
    uint16_t GetRefreshRate() const { return m_Settings.MonitorRefreshRate; }

    void SetSize( int32_t width, int32_t height );
    void SetMinimumSize( const Vector2s32& minSize );
    void SetMaximumSize( const Vector2s32& maxSize );
    void SetWindowMode( WindowMode mode );
    void SetState( WindowState state );

    bool GetRequiredExtensions( std::vector< const char* >& outExtensions ) const;

    bool IsVSyncEnabled() const { return m_Settings.UseVsync; }
    bool IsMinimized() const { return m_State == WindowState::Minimized; }

    int32_t GetWidth() const { return m_Settings.Size.X; }
    int32_t GetHeight() const { return m_Settings.Size.Y; }

    void SetCursor( void* /*cursor*/ ) {}

    void EnableSystemMouseCapture( [[maybe_unused]] bool enable ) {}
    WaylandPlatformContext& GetContext() const { return *m_Context; }
    wl_surface* GetSurfaceHandle() const { return m_Surface; }

    WindowSettings& GetSettings() { return m_Settings; }
    const WindowSettings& GetSettings() const { return m_Settings; }

    Sink< ResizeSignalT > OnResize() const { return Sink( m_ResizeSignal ); }
    Sink< FocusSignalT > OnFocus() { return Sink( m_FocusSignal ); }
    Sink< CloseSignalT > OnClose() { return Sink( m_CloseSignal ); }

    WindowState getState() const { return m_State; }
    uint32_t getId() const { return m_id; }

  private:
    static void HandleSurfaceConfigure( void* data, xdg_surface* surface, uint32_t serial );
    static void HandleToplevelConfigure( void* data,
                                         xdg_toplevel* toplevel,
                                         int32_t width,
                                         int32_t height,
                                         wl_array* states );
    static void HandleToplevelClose( void* data, xdg_toplevel* xdg_toplevel );
    static void HandleTopLevelDecorationConfigure( void* data,
                                                   zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1,
                                                   uint32_t mode );

    static void HandleSurfaceEnter( void* data, wl_surface* surface, wl_output* output );
    static void HandleSurfaceLeave( void* data, wl_surface* surface, wl_output* output );

    void CreateNativeWindow();
    void FitToMonitor() {}
    void CaptureCursor() {}
    void ReleaseCursor() {}

  private:
    WindowSettings m_Settings;

    mutable ResizeSignalT m_ResizeSignal;
    FocusSignalT m_FocusSignal;
    CloseSignalT m_CloseSignal;

    Atomic< bool > m_IsInitialized = false;
    WindowState m_State = WindowState::None;

    uint32_t m_id;
    WaylandPlatformContext* m_Context = nullptr;

    wl_surface* m_Surface = nullptr;
    xdg_surface* m_XdgSurface = nullptr;

    xdg_toplevel* m_XdgToplevel = nullptr;
    zxdg_toplevel_decoration_v1* m_XdgTopLevelDecoration = nullptr;
};
} // namespace onyx::platform::wayland
#endif
