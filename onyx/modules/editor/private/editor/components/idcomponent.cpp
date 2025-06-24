#include <onyx/gamecore/components/idcomponent.h>

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
#include <imgui.h>
#include <imgui_internal.h>
#endif

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool IdComponent::DrawImGuiEditor()
    {
        ImGui::TextEx(Format::Format("{}", Id), 0, ImGuiInputTextFlags_ReadOnly);
        return false;
    }
#endif
}
