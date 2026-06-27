#include <onyx/ui/scopedid.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace onyx::ui {
ScopedImGuiId::ScopedImGuiId( StringView str ) {
    ImGui::PushID( str.data() );
}

ScopedImGuiId::ScopedImGuiId( int32_t id ) {
    ImGui::PushID( id );
}

ScopedImGuiId::ScopedImGuiId( uint32_t id ) {
    ImGui::PushID( numericCast< int32_t >( id ) );
}

ScopedImGuiId::~ScopedImGuiId() {
    ImGui::PopID();
}
} // namespace onyx::ui
#endif