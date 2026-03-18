#include <imgui_stacklayout.h>
#include <onyx/ui/layout/imguilayout.h>

namespace onyx::ui::layout
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
