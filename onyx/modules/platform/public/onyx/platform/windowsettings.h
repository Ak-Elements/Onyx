#pragma once

#include <onyx/platform/platformfwd.h>

namespace onyx::platform {
using ResizeSignalT = Signal< void( Vector2s32 ) >;
using FocusSignalT = Signal< void( bool ) >;
using CloseSignalT = Signal< void( Window& ) >;

enum class WindowMode : uint8_t { Windowed, Fullscreen, Borderless };

enum class WindowState : uint8_t {
    None,
    Hidden,
    Minimized,
    Background,
    Default,
    Maximized,
    Resizing,
    Closed,
};

enum class CursorMode : uint8_t { Normal, Captured, Disabled };

struct WindowSettings {
    WindowSettings() = default;
    WindowSettings( StringView title, const Vector2u32& size, WindowMode mode )
        : Mode( mode )
        , Size( size )
        , Title( title ) {}

    WindowMode Mode = WindowMode::Windowed;
    int8_t MonitorIndex = -1;

    Vector2s32 Position{ -1, -1 };
    Vector2s32 Size = { 1024, 768 };

    Vector2s32 MinSize{ 0, 0 };
    Vector2s32 MaxSize{ 0, 0 };

    uint16_t MonitorRefreshRate = 60;
    String Title = "Onyx";
    void* Icon = nullptr;
    void* SmallIcon = nullptr;

    bool AutoMinimize = true;
    bool HasMenu = false;
    bool HasTitleBar = true;
    bool IsTransparent = false;
    bool UseVsync = true;
};
} // namespace onyx::platform

namespace onyx {
template <> struct Serialization< platform::WindowSettings > {
    static bool serialize( Serializer& serializer, const platform::WindowSettings& settings );
    static bool deserialize( const Deserializer& deserializer, platform::WindowSettings& outSettings );
};
} // namespace onyx
