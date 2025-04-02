#pragma once

#if ONYX_IS_WINDOWS

namespace Onyx::Platform
{
    struct AdapterInfo
    {
        String m_Name;
        DynamicArray<StringId> m_Monitors;
    };

    void GetAdapterAndMonitorInfos(HashMap<StringId, AdapterInfo>& outAdapterInfo);
    onyxS32 GetMonitorIndexByName(const String& name);

    Guid CreateGuid();
}

#endif