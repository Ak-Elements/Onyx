#pragma once

#if ONYX_IS_WINDOWS

namespace onyx::platform {
struct AdapterInfo {
    String m_Name;
    DynamicArray< StringId32 > m_Monitors;
};

void GetAdapterAndMonitorInfos( HashMap< StringId32, AdapterInfo >& outAdapterInfo );
int32_t GetMonitorIndexByName( const String& name );

Vector2s32 GetMousePosition();
Vector3u8 GetPixelColorAtMousePosition();
Vector3u8 GetPixelColor( const Vector2s32& screenPosition );

Guid CreateGuid();
} // namespace onyx::platform

#endif