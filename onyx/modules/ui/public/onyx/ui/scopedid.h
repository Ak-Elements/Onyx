#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui {
struct ScopedImGuiId {
    ScopedImGuiId( StringView str );
    ScopedImGuiId( int32_t id );
    ScopedImGuiId( uint32_t id );
    ~ScopedImGuiId();
};
} // namespace onyx::ui
#endif