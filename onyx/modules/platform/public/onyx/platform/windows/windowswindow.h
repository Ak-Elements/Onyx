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

namespace Onyx::Platform::Windows
{
    class PlatformContext;

    class Window : public Thread
    {
    public:
        // 400 is WM_USER
        static constexpr onyxU32 ONYX_WM_SYSTEM_MOUSEHOOK = 0x0400 + 1;

        static constexpr onyxU32 ONYX_WM_SYSTEM_PRIMARY_MOUSEDOWN = 0x0400 + 2;
        static constexpr onyxU32 ONYX_WM_SYSTEM_PRIMARY_MOUSEUP = 0x0400 + 3;

        Window(PlatformContext& context, const WindowSettings& settings);
        ~Window() override;

        void Show();
        void Hide();

        void Minimize();
        void Maximize();
        void Focus();
        void RequestWindowAttention();

        void ToggleCursor(bool enable);
        void UpdateCursorImage();

        bool IsInitialized() const { return m_IsInitialized; }

        const PlatformContext& GetContext() const { ONYX_ASSERT(m_Context != nullptr); return *m_Context; }
        HWND GetWindowHandle() const { return m_WindowHandle; }

        void SetTitle(StringView m_title);
        void SetIcon(const FilePath& path);

        Vector2s32 GetFrameBufferSize() const { return m_Settings.Size; }
        onyxU16 GetRefreshRate() const { return m_Settings.MonitorRefreshRate; }

        void SetSize(onyxS32 width, onyxS32 height);
        void SetMinimumSize(const Vector2s32& minSize);
        void SetMaximumSize(const Vector2s32& minSize);
        void SetWindowMode(WindowMode mode);
        void SetState(WindowState state);

        bool GetRequiredExtensions(std::vector<const char*>& outExtensions) const;

        bool IsVSyncEnabled() const { return m_Settings.UseVsync; }
        bool IsMinimized() const { return m_State == WindowState::Minimized; }

        onyxS32 GetWidth() const { return m_Settings.Size[0]; }
        onyxS32 GetHeight() const { return m_Settings.Size[1]; }

        void SetCursor(HCURSOR cursor);
        void EnableSystemMouseCapture(bool enable);

        WindowSettings& GetSettings() { return m_Settings; }
        const WindowSettings& GetSettings() const { return m_Settings; }

        Sink<ResizeSignalT> OnResize() const { return Sink(m_ResizeSignal); }
        Sink<FocusSignalT> OnFocus() { return Sink(m_FocusSignal); }
        Sink<CloseSignalT> OnClose() { return Sink(m_CloseSignal); }

    private:
        static onyxS64 __stdcall OnWindowProc(HWND hWnd, onyxU32 message, onyxU64 wParam, onyxS64 lParam);

        void OnStart() override;

        bool HandleWindowMessages(onyxU32 messageType, onyxU64 wParam, onyxS64 lParam);
        void OnUpdate() override;
        void OnStop() override;

        void CreateNativeWindow();

        void FitToMonitor();

        void CaptureCursor();
        void ReleaseCursor();

        onyxS32 GetStyle();
        onyxS32 GetExtendedStyle();

    private:
        WindowSettings m_Settings;

        mutable ResizeSignalT m_ResizeSignal;
        FocusSignalT m_FocusSignal;
        CloseSignalT m_CloseSignal;

        PlatformContext* m_Context = nullptr;

        Atomic<bool> m_IsInitialized = false;
        WindowState m_State = WindowState::None;

        HCURSOR m_Cursor = nullptr;
        CursorMode m_CursorMode = CursorMode::Normal;

        HWND m_WindowHandle = nullptr;
        void* myWakeFromSleepEvent = nullptr;

        HHOOK m_SystemMouseHook = nullptr;
    };
}

#endif