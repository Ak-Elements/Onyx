#pragma once

namespace Onyx::Graphics
{
    enum class WindowMode : onyxU8
    {
        Windowed,
        Fullscreen,
        Borderless
    };

    enum class WindowState : onyxU8
    {
        None,
        Hidden,
        Minimized,
        Background,
        Default,
        Maximized,
        Resizing,
    };

    enum class CursorMode : onyxU8
    {
        Normal,
        Captured,
        Disabled
    };

    struct WindowSettings
    {
        WindowSettings() = default;
        WindowSettings(const StringView& title, const Vector2u32& size, WindowMode mode)
            : Mode(mode)
            , Size(size)
            , Title(title)
        {
        }

        WindowMode Mode = WindowMode::Windowed;
        onyxS8 MonitorIndex = -1;

        Vector2s32 Position{ -1 ,-1 };
        Vector2u32 Size = { 0, 0 };

        Vector2s32 MinSize{ 0, 0 };
        Vector2s32 MaxSize{ 0, 0 };

        onyxU16 MonitorRefreshRate = 60;
        String Title = "";
        void* Icon = nullptr;
        void* SmallIcon = nullptr;

        bool AutoMinimize = true;
        bool HasMenu = false;
        bool HasTitleBar = true;
        bool IsTransparent = false;
        bool UseVsync = true;
    };
}

#if ONYX_USE_SDL2
#include <onyx/graphics/window/sdlwindow.h>
#elif ONYX_IS_WINDOWS
#include <onyx/graphics/window/windows/nativewindow.h>
#else
static_assert(false, "Unsupported window library.");
#endif
