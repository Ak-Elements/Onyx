#pragma once

#if ONYX_USE_IMGUI

namespace Onyx::Ui
{
    struct ScopedImGuiDisabled
    {
        ScopedImGuiDisabled();
        ~ScopedImGuiDisabled();
    };
}
#endif