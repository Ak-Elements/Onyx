#include <onyx/ui/controls/combobox.h>

#include <onyx/localization/localizedstring.h>

#include <imgui.h>

namespace onyx::ui
{
    bool BeginCombobox(StringView id, const localization::LocalizedString& label)
    {
        return BeginCombobox(id, label.Get());
    }

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
