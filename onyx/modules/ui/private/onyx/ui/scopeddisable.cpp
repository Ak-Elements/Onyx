#include <onyx/ui/scopeddisable.h>
#include <imgui.h>

namespace Onyx::Ui
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
