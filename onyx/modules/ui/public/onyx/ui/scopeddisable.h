#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui {
struct ScopedImGuiDisabled {
    ScopedImGuiDisabled();
    ScopedImGuiDisabled( bool isDisabled );
    ~ScopedImGuiDisabled();
};
} // namespace onyx::ui
#endif
