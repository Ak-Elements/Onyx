#pragma once

#if ONYX_IS_WINDOWS

#include <onyx/platform/windowsettings.h>
#include <onyx/thread/thread.h>

struct HWND__;
typedef HWND__* HWND;

struct HHOOK__;
typedef HHOOK__* HHOOK;

struct HICON__;
typedef HICON__* HICON;
typedef HICON HCURSOR;

namespace onyx::platform::windows {
class PlatformContext;

class Window : public Thread {
  public:
    // 400 is WM_USER
    static constexpr uint32_t ONYX_WM_SYSTEM_MOUSEHOOK = 0x0400 + 1;

    static constexpr uint32_t ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN = 0x0400 + 2;
    static constexpr uint32_t ONYX_WM_SYSTEM_PRIMARY_MOUSEUP = 0x0400 + 3;

    Window( PlatformContext& context, const WindowSettings& settings );
    ~Window() override;

    void Show();
    void Hide();

    void Minimize();
    void Maximize();
    void Focus();
    void RequestWindowAttention();

    void ToggleCursor( bool enable );
    void UpdateCursorImage();

    bool IsInitialized() const { return m_IsInitialized; }

    const PlatformContext& GetContext() const {
        ONYX_ASSERT( m_Context != nullptr );
        return *m_Context;
    }
    HWND GetWindowHandle() const { return m_WindowHandle; }

    void SetTitle( StringView m_title );
    void SetIcon( const FilePath& path );

    Vector2s32 GetFrameBufferSize() const { return m_Settings.Size; }
    uint16_t GetRefreshRate() const { return m_Settings.MonitorRefreshRate; }

    void SetSize( int32_t width, int32_t height );
    void SetMinimumSize( const Vector2s32& minSize );
    void SetMaximumSize( const Vector2s32& minSize );
    void SetWindowMode( WindowMode mode );
    void SetState( WindowState state );

    bool GetRequiredExtensions( std::vector< const char* >& outExtensions ) const;

    bool IsVSyncEnabled() const { return m_Settings.UseVsync; }
    bool IsMinimized() const { return m_State == WindowState::Minimized; }

    int32_t GetWidth() const { return m_Settings.Size[ 0 ]; }
    int32_t GetHeight() const { return m_Settings.Size[ 1 ]; }

    void SetCursor( HCURSOR cursor );
    void EnableSystemMouseCapture( bool enable );

    WindowSettings& GetSettings() { return m_Settings; }
    const WindowSettings& GetSettings() const { return m_Settings; }

    Sink< ResizeSignalT > OnResize() const { return Sink( m_ResizeSignal ); }
    Sink< FocusSignalT > OnFocus() { return Sink( m_FocusSignal ); }
    Sink< CloseSignalT > OnClose() { return Sink( m_CloseSignal ); }

  private:
    static int64_t __stdcall OnWindowProc( HWND hWnd, uint32_t message, uint64_t wParam, int64_t lParam );

    void OnStart() override;

    bool HandleWindowMessages( uint32_t messageType, uint64_t wParam, int64_t lParam );
    void OnUpdate() override;
    void OnStop() override;

    void CreateNativeWindow();

    void FitToMonitor();

    void CaptureCursor();
    void ReleaseCursor();

    int32_t GetStyle();
    int32_t GetExtendedStyle();

  private:
    WindowSettings m_Settings;

    mutable ResizeSignalT m_ResizeSignal;
    FocusSignalT m_FocusSignal;
    CloseSignalT m_CloseSignal;

    PlatformContext* m_Context = nullptr;

    Atomic< bool > m_IsInitialized = false;
    WindowState m_State = WindowState::None;

    HCURSOR m_Cursor = nullptr;
    CursorMode m_CursorMode = CursorMode::Normal;

    HWND m_WindowHandle = nullptr;
    void* myWakeFromSleepEvent = nullptr;

    HHOOK m_SystemMouseHook = nullptr;
};
} // namespace onyx::platform::windows

#endif