#include <imgui.h>
#include <onyx/ui/scopeddisable.h>

namespace onyx::ui {
ScopedImGuiDisabled::ScopedImGuiDisabled() {
    ImGui::BeginDisabled();
}

ScopedImGuiDisabled::ScopedImGuiDisabled( bool isDisabled ) {
    ImGui::BeginDisabled( isDisabled );
}

ScopedImGuiDisabled::~ScopedImGuiDisabled() {
    ImGui::EndDisabled();
}

} // namespace onyx::ui
