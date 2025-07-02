#include <imgui_stacklayout.h>
#include <onyx/ui/layout/imguilayout.h>

namespace Onyx::Ui::Layout
{
    void BeginHorizontal(StringView id)
    {
        ImGui::BeginHorizontal(id.data());
    }

    void EndHorizontal()
    {
        ImGui::EndHorizontal();
    }
}
