#include <onyx/ui/scopeddisable.h>
#include <imgui.h>

namespace onyx::ui
{
    ScopedImGuiDisabled::ScopedImGuiDisabled()
    {
        ImGui::BeginDisabled();
    }

    ScopedImGuiDisabled::~ScopedImGuiDisabled()
    {
        ImGui::EndDisabled();
    }


}
