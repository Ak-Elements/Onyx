#include <onyx/ui/scopedid.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace Onyx::Ui
{
    ScopedImGuiId::ScopedImGuiId(StringView str)
    {
        ImGui::PushID(str.data());
    }

    ScopedImGuiId::ScopedImGuiId(onyxS32 id)
    {
        ImGui::PushID(id);
    }

    ScopedImGuiId::~ScopedImGuiId()
    {
        ImGui::PopID();
    }
}
#endif