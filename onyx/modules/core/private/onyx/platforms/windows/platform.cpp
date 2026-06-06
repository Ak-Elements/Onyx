
#include <onyx/hash.h>
#include <onyx/log/logger.h>
#include <onyx/platforms/windows/platform.h>

#if ONYX_IS_WINDOWS

#include <combaseapi.h>
#include <Windows.h>

namespace onyx::platform {
void GetAdapterAndMonitorInfos( HashMap< StringId32, AdapterInfo >& outAdapterInfo ) {
    constexpr uint64_t DISPLAY_DEVICE_SIZE = sizeof( DISPLAY_DEVICE );

    DISPLAY_DEVICE displayDevice = {};
    ::ZeroMemory( &displayDevice, DISPLAY_DEVICE_SIZE );
    displayDevice.cb = DISPLAY_DEVICE_SIZE;

    int16_t adapterIndex = -1;
    while ( ::EnumDisplayDevices( nullptr, ++adapterIndex, &displayDevice, 0 ) ) {
        if ( ( displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE ) != DISPLAY_DEVICE_ACTIVE )
            continue;

        StringId32 adapterHash( StringView( displayDevice.DeviceString ) );
        AdapterInfo& adapterInfo = outAdapterInfo[ adapterHash ];
        adapterInfo.m_Name = displayDevice.DeviceString;
        adapterInfo.m_Monitors.emplace_back( StringId32( StringView( displayDevice.DeviceName ) ) );
    }
}

int32_t GetMonitorIndexByName( const String& name ) {
    StringId32 monitorNameHash( name );

    constexpr uint64_t DISPLAY_DEVICE_SIZE = sizeof( DISPLAY_DEVICE );

    DISPLAY_DEVICE displayDevice = {};
    ::ZeroMemory( &displayDevice, DISPLAY_DEVICE_SIZE );
    displayDevice.cb = DISPLAY_DEVICE_SIZE;

    int16_t adapterIndex = -1;
    while ( ::EnumDisplayDevices( nullptr, ++adapterIndex, &displayDevice, 0 ) ) {
        if ( ( displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE ) != DISPLAY_DEVICE_ACTIVE )
            continue;

        if ( monitorNameHash == StringId32( StringView( displayDevice.DeviceName ) ) ) {
            return adapterIndex;
        }
    }

    return -1;
}

Vector2s32 GetMousePosition() {
    POINT mousePosition{};
    ::GetCursorPos( &mousePosition );

    return { static_cast< int32_t >( mousePosition.x ), static_cast< int32_t >( mousePosition.y ) };
}

Vector3u8 GetPixelColorAtMousePosition() {
    return GetPixelColor( GetMousePosition() );
}

Vector3u8 GetPixelColor( const Vector2s32& screenPosition ) {
    HDC dc = GetDC( nullptr );

    COLORREF color = ::GetPixel( dc, static_cast< int >( screenPosition.X ), static_cast< int >( screenPosition.Y ) );
    ReleaseDC( nullptr, dc );

    uint8_t red = static_cast< uint8_t >( color & 0xff );
    uint8_t green = static_cast< uint8_t >( ( color >> 8 ) & 0xff );
    uint8_t blue = static_cast< uint8_t >( ( color >> 16 ) & 0xff );

    return Vector3u8{ red, green, blue };
}

int32_t GetPrimaryMonitor() {
    constexpr uint64_t DISPLAY_DEVICE_SIZE = sizeof( DISPLAY_DEVICE );

    DISPLAY_DEVICE displayDevice = {};
    ::ZeroMemory( &displayDevice, DISPLAY_DEVICE_SIZE );
    displayDevice.cb = DISPLAY_DEVICE_SIZE;

    int16_t adapterIndex = -1;
    while ( ::EnumDisplayDevices( nullptr, ++adapterIndex, &displayDevice, 0 ) ) {
        if ( ( displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE ) != DISPLAY_DEVICE_ACTIVE )
            continue;

        if ( ( displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != DISPLAY_DEVICE_PRIMARY_DEVICE )
            continue;

        return adapterIndex;
    }

    return -1;
}

Guid CreateGuid() {
    GUID gidReference;
    HRESULT hCreateGuid = ::CoCreateGuid( &gidReference );
    ONYX_ASSERT( hCreateGuid == SEVERITY_SUCCESS );
    std::ignore = hCreateGuid;

    return {
        static_cast< uint64_t >( gidReference.Data4[ 7 ] ) << 7 |
            static_cast< uint64_t >( gidReference.Data4[ 6 ] ) << 6 |
            static_cast< uint64_t >( gidReference.Data4[ 5 ] ) << 5 |
            static_cast< uint64_t >( gidReference.Data4[ 4 ] ) << 4 |
            static_cast< uint64_t >( gidReference.Data4[ 3 ] ) << 3 |
            static_cast< uint64_t >( gidReference.Data4[ 2 ] ) << 2 |
            static_cast< uint64_t >( gidReference.Data4[ 1 ] ) << 1 |
            static_cast< uint64_t >( gidReference.Data4[ 0 ] ), // Low part
        static_cast< uint64_t >( gidReference.Data1 ) << 4 | static_cast< uint64_t >( gidReference.Data2 ) << 2 |
            static_cast< uint64_t >( gidReference.Data3 ), // High part

    };
}
} // namespace onyx::platform
#endif