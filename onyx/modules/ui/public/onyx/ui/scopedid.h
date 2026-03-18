#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui
{
    struct ScopedImGuiId
    {
        ScopedImGuiId(StringView str);
        ScopedImGuiId(onyxS32 id);
        ~ScopedImGuiId();
    };
}
#endif