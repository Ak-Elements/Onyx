#pragma once

#if ONYX_USE_IMGUI

namespace Onyx::Ui
{
    struct ScopedImGuiId
    {
        ScopedImGuiId(StringView str);
        ScopedImGuiId(onyxS32 id);
        ~ScopedImGuiId();
    };
}
#endif