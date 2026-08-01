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
    Window( uint32_t id, WaylandPlatformContext& platformContext, WindowSettings setting );
    ~Window();

    void show();
    void hide();

    void close();

    void minimize();
    void maximize();
    void focus() {}
    void requestWindowAttention() {}

    void toggleCursor( bool enable ) {}
    void updateCursorImage() {}

    void setTitle( StringView title ) { m_settings.Title = String( title ); }
    void setIcon( const FilePath& path ) {}

    Vector2s32 getFrameBufferSize() const { return m_settings.Size; }
    uint16_t getRefreshRate() const { return m_settings.MonitorRefreshRate; }

    void setSize( int32_t width, int32_t height );
    void setMinimumSize( const Vector2s32& minSize );
    void setMaximumSize( const Vector2s32& maxSize );
    void setWindowMode( WindowMode mode );
    void setState( WindowState state );

    bool getRequiredExtensions( std::vector< const char* >& outExtensions ) const;

    bool isVSyncEnabled() const { return m_settings.UseVsync; }
    bool isMinimized() const { return m_state == WindowState::Minimized; }

    int32_t getWidth() const { return m_settings.Size.X; }
    int32_t getHeight() const { return m_settings.Size.Y; }

    void setCursor( void* /*cursor*/ ) {}

    void enableSystemMouseCapture( [[maybe_unused]] bool enable ) {}
    WaylandPlatformContext& getContext() const { return *m_context; }
    wl_surface* getSurfaceHandle() const { return m_surface; }

    WindowSettings& getSettings() { return m_settings; }
    const WindowSettings& getSettings() const { return m_settings; }

    Sink< ResizeSignalT > onResize() const { return Sink( m_resizeSignal ); }
    Sink< FocusSignalT > onFocus() { return Sink( m_focusSignal ); }
    Sink< CloseSignalT > onClose() { return Sink( m_closeSignal ); }

    WindowState getState() const { return m_state; }
    uint32_t getId() const { return m_id; }

  private:
    static void handleSurfaceConfigure( void* data, xdg_surface* surface, uint32_t serial );
    static void handleToplevelConfigure( void* data,
                                         xdg_toplevel* toplevel,
                                         int32_t width,
                                         int32_t height,
                                         wl_array* states );
    static void handleToplevelClose( void* data, xdg_toplevel* xdgToplevel );
    static void handleTopLevelDecorationConfigure( void* data,
                                                   zxdg_toplevel_decoration_v1* zxdgToplevelDecorationV1,
                                                   uint32_t mode );

    static void handleSurfaceEnter( void* data, wl_surface* surface, wl_output* output );
    static void handleSurfaceLeave( void* data, wl_surface* surface, wl_output* output );

    void createNativeWindow();
    void fitToMonitor() {}
    void captureCursor() {}
    void releaseCursor() {}

  private:
    WindowSettings m_settings;

    mutable ResizeSignalT m_resizeSignal;
    FocusSignalT m_focusSignal;
    CloseSignalT m_closeSignal;

    Atomic< bool > m_isInitialized = false;
    WindowState m_state = WindowState::None;

    uint32_t m_id;
    WaylandPlatformContext* m_context = nullptr;

    wl_surface* m_surface = nullptr;
    xdg_surface* m_xdgSurface = nullptr;

    xdg_toplevel* m_xdgToplevel = nullptr;
    zxdg_toplevel_decoration_v1* m_xdgTopLevelDecoration = nullptr;
};
} // namespace onyx::platform::wayland
#endif
