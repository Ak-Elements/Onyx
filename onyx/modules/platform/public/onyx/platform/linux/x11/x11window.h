#pragma once

#if ONYX_IS_LINUX && ONYX_USE_X11

#include <onyx/thread/thread.h>

#include <onyx/eventhandler.h>
#include <onyx/platform/windowsettings.h>

struct xcb_intern_atom_reply_t;

namespace onyx::platform::x11 {
class X11PlatformContext;

class Window {
  public:
    Window( uint32_t id, X11PlatformContext& platformContext, WindowSettings setting );
    ~Window();

    void Show();
    void Hide();

    void Minimize();
    void Maximize();
    void Focus() {}
    void RequestWindowAttention() {}

    void ToggleCursor( bool enable ) {}
    void UpdateCursorImage() {}

    void SetTitle( StringView title ) { m_settings.Title = String( title ); }
    void SetIcon( const FilePath& path ) {}

    Vector2s32 GetFrameBufferSize() const { return m_settings.Size; }
    uint16_t GetRefreshRate() const { return m_settings.MonitorRefreshRate; }

    void SetSize( int32_t width, int32_t height );
    void SetMinimumSize( const Vector2s32& minSize );
    void SetMaximumSize( const Vector2s32& maxSize );
    void SetWindowMode( WindowMode mode );
    void SetState( WindowState state );

    bool GetRequiredExtensions( std::vector< const char* >& outExtensions ) const;

    bool IsVSyncEnabled() const { return m_settings.UseVsync; }
    bool IsMinimized() const { return m_state == WindowState::Minimized; }

    uint32_t getId() const { return m_window; }
    int32_t GetWidth() const { return m_settings.Size.X; }
    int32_t GetHeight() const { return m_settings.Size.Y; }

    void SetCursor( void* /*cursor*/ ) {}

    void EnableSystemMouseCapture( bool enable ) { ONYX_UNUSED( enable ); }
    X11PlatformContext& GetContext() const { return *m_context; }
    uint32_t GetSurfaceHandle() const { return m_window; }

    WindowSettings& GetSettings() { return m_settings; }
    const WindowSettings& GetSettings() const { return m_settings; }

    WindowState getState() const { return m_state; }

    Sink< ResizeSignalT > OnResize() const { return Sink( m_resizeSignal ); }
    Sink< FocusSignalT > OnFocus() { return Sink( m_focusSignal ); }
    Sink< CloseSignalT > OnClose() { return Sink( m_closeSignal ); }

  private:
    bool initConnection();
    void createWindow();

    void fitToMonitor() {}
    void captureCursor() {}
    void releaseCursor() {}

  private:
    WindowSettings m_settings;
    X11PlatformContext* m_context = nullptr;

    mutable ResizeSignalT m_resizeSignal;
    FocusSignalT m_focusSignal;
    CloseSignalT m_closeSignal;

    WindowState m_state = WindowState::None;

    uint32_t m_window = 0;
};
} // namespace onyx::platform::x11
#endif