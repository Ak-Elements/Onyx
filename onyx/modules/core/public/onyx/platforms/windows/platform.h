#pragma once

#if ONYX_IS_WINDOWS

namespace Onyx::Platform
{
    struct AdapterInfo
    {
        String m_Name;
        DynamicArray<StringId32> m_Monitors;
    };

    void GetAdapterAndMonitorInfos(HashMap<StringId32, AdapterInfo>& outAdapterInfo);
    onyxS32 GetMonitorIndexByName(const String& name);

    Guid CreateGuid();
}

#endif