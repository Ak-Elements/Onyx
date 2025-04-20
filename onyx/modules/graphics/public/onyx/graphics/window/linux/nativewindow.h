#pragma once

#if ONYX_IS_LINUX

#include <onyx/filesystem/path.h>
#include <onyx/eventhandler.h>

namespace Onyx::Graphics
{
    class Window
    {
        using WindowMessageHandler = InplaceFunction<bool(onyxU32, onyxU64, onyxU64)>;

    public:
        ~Window() = default;

        void Create(const WindowSettings& settings) { m_Settings = settings; m_IsInitialized = true; m_NativeHandle = reinterpret_cast<void*>(0xDEADBEEF); } // mock
        void Destroy() { m_IsInitialized = false; m_NativeHandle = nullptr; }

        void Show() {}
        void Hide() {}

        void Minimize() { m_State = WindowState::Minimized; }
        void Maximize() { m_State = WindowState::Maximized; }
        void Focus() {}
        void RequestWindowAttention() {}

        void ToggleCursor(bool enable) {}
        void UpdateCursorImage() {}

        void SetTitle(const StringView& title) { m_Settings.Title = std::string(title); }
        void SetIcon(const FileSystem::Filepath& path) {}

        Vector2u32 GetFrameBufferSize() const { return m_Settings.Size; }
        onyxU16 GetRefreshRate() const { return m_Settings.MonitorRefreshRate; }

        void SetSize(onyxU32 width, onyxU32 height) { m_Settings.Size = {width, height}; }
        void SetMinimumSize(const Vector2s32& minSize) {}
        void SetMaximumSize(const Vector2s32& maxSize) {}
        void SetWindowMode(WindowMode mode) {}
        void SetState(WindowState state) { m_State = state; }

        bool GetRequiredExtensions(std::vector<const char*>& outExtensions) const {
            outExtensions.push_back("VK_KHR_surface");
            outExtensions.push_back("VK_KHR_xcb_surface"); // Swap depending on Linux backend
            return true;
        }

        bool IsVSyncEnabled() const { return m_Settings.UseVsync; }
        bool IsMinimized() const { return m_State == WindowState::Minimized; }

        onyxU32 GetWidth() const { return m_Settings.Size[0]; }
        onyxU32 GetHeight() const { return m_Settings.Size[1]; }

        void SetWindowMessageHandler(const WindowMessageHandler& handler) { m_WindowMessageHandler = handler; }
        void ClearWindowMessageHandler() { m_WindowMessageHandler = nullptr; }
        void SetCursor(void* cursor) { m_Cursor = cursor; }

        void* GetWindowHandle() const { return m_NativeHandle; }

        ONYX_EVENT(OnFocus, bool);
        ONYX_EVENT(OnResize, onyxU32, onyxU32);
        ONYX_EVENT(OnClose);

    private:
        bool HandleWindowMessages(onyxU32, onyxU64, onyxS64) { return false; }

        void CreateNativeWindow() {}
        void FitToMonitor() {}
        void CaptureCursor() {}
        void ReleaseCursor() {}

        onyxS32 GetStyle() { return 0; }
        onyxS32 GetExtendedStyle() { return 0; }

    private:
        WindowState m_State = WindowState::None;
        bool m_IsInitialized = false;
    
        WindowSettings m_Settings;

        void* m_Cursor = nullptr;
        void* m_NativeHandle = nullptr;

        WindowMessageHandler m_WindowMessageHandler;
    };
}
#endif