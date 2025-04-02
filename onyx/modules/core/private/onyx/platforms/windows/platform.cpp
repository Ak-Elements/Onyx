
#include <onyx/hash.h>
#include <onyx/log/logger.h>
#include <onyx/platforms/windows/platform.h>

#if ONYX_IS_WINDOWS

#include <Windows.h>
#include <combaseapi.h>

namespace Onyx::Platform
{
    void GetAdapterAndMonitorInfos(HashMap<StringId, AdapterInfo>& outAdapterInfo)
    {
        constexpr onyxU64 DISPLAY_DEVICE_SIZE = sizeof(DISPLAY_DEVICE);

        DISPLAY_DEVICE displayDevice = {};
        ::ZeroMemory(&displayDevice, DISPLAY_DEVICE_SIZE);
        displayDevice.cb = DISPLAY_DEVICE_SIZE;

        onyxS16 adapterIndex = -1;
        while (::EnumDisplayDevices(nullptr, ++adapterIndex, &displayDevice, 0))
        {
            if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) != DISPLAY_DEVICE_ACTIVE)
                continue;

            StringId adapterHash = Hash::FNV1aHash32(displayDevice.DeviceString, 128, 0);
            AdapterInfo& adapterInfo = outAdapterInfo[adapterHash];
            adapterInfo.m_Name = displayDevice.DeviceString;
            adapterInfo.m_Monitors.emplace_back(Hash::FNV1aHash32(displayDevice.DeviceName, 32, 0));
        }
    }

    onyxS32 GetMonitorIndexByName(const String& name)
    {
        onyxU32 monitorNameHash = Hash::FNV1aHash32(name);

        constexpr onyxU64 DISPLAY_DEVICE_SIZE = sizeof(DISPLAY_DEVICE);

        DISPLAY_DEVICE displayDevice = {};
        ::ZeroMemory(&displayDevice, DISPLAY_DEVICE_SIZE);
        displayDevice.cb = DISPLAY_DEVICE_SIZE;

        onyxS16 adapterIndex = -1;
        while (::EnumDisplayDevices(nullptr, ++adapterIndex, &displayDevice, 0))
        {
            if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) != DISPLAY_DEVICE_ACTIVE)
                continue;

            if (monitorNameHash == Hash::FNV1aHash32(displayDevice.DeviceName, 32, 0))
            {
                return adapterIndex;
            }
        }

        return -1;
    }

    onyxS32 GetPrimaryMonitor()
    {
        constexpr onyxU64 DISPLAY_DEVICE_SIZE = sizeof(DISPLAY_DEVICE);

        DISPLAY_DEVICE displayDevice = {};
        ::ZeroMemory(&displayDevice, DISPLAY_DEVICE_SIZE);
        displayDevice.cb = DISPLAY_DEVICE_SIZE;

        onyxS16 adapterIndex = -1;
        while (::EnumDisplayDevices(nullptr, ++adapterIndex, &displayDevice, 0))
        {
            if ((displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE) != DISPLAY_DEVICE_ACTIVE)
                continue;

            if ((displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != DISPLAY_DEVICE_PRIMARY_DEVICE)
                continue;

            return adapterIndex;
        }

        return -1;
    }

    Guid CreateGuid()
    {
        GUID gidReference;
        HRESULT hCreateGuid = ::CoCreateGuid(&gidReference);
        ONYX_ASSERT(hCreateGuid == SEVERITY_SUCCESS);
        ONYX_UNUSED(hCreateGuid);

        return {
            static_cast<onyxU64>(gidReference.Data4[7]) << 7 | static_cast<onyxU64>(gidReference.Data4[6]) << 6 | static_cast<onyxU64>(gidReference.Data4[5]) << 5 |
            static_cast<onyxU64>(gidReference.Data4[4]) << 4 | static_cast<onyxU64>(gidReference.Data4[3]) << 3 | static_cast<onyxU64>(gidReference.Data4[2]) << 2 |
            static_cast<onyxU64>(gidReference.Data4[1]) << 1 | static_cast<onyxU64>(gidReference.Data4[0]),                     // Low part
            static_cast<onyxU64>(gidReference.Data1) << 4 | static_cast<onyxU64>(gidReference.Data2) << 2 | static_cast<onyxU64>(gidReference.Data3), // High part

        };
    }
}
#endif