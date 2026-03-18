#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui
{
    struct ScopedImGuiDisabled
    {
        ScopedImGuiDisabled();
        ~ScopedImGuiDisabled();
    };
}
#endif