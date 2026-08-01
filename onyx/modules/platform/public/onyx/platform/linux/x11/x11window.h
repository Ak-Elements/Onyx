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

    void show();
    void hide();

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

    uint32_t getId() const { return m_window; }
    int32_t getWidth() const { return m_settings.Size.X; }
    int32_t getHeight() const { return m_settings.Size.Y; }

    void setCursor( void* /*cursor*/ ) {}

    void enableSystemMouseCapture( [[maybe_unused]] bool enable ) {}
    X11PlatformContext& getContext() const { return *m_context; }
    uint32_t getSurfaceHandle() const { return m_window; }

    WindowSettings& getSettings() { return m_settings; }
    const WindowSettings& getSettings() const { return m_settings; }

    WindowState getState() const { return m_state; }

    Sink< ResizeSignalT > onResize() const { return Sink( m_resizeSignal ); }
    Sink< FocusSignalT > onFocus() { return Sink( m_focusSignal ); }
    Sink< CloseSignalT > onClose() { return Sink( m_closeSignal ); }

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
