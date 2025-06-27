#include <onyx/ui/controls/combobox.h>

#include <imgui.h>

namespace Onyx::Ui
{
    bool BeginCombobox(StringView id, StringView label)
    {
        bool hasChanged = ImGui::BeginCombo(id.data(), label.data());
        return hasChanged;
    }

    void EndCombobox()
    {
        ImGui::EndCombo();
    }

}
