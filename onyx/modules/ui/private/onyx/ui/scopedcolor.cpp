#include <onyx/ui/scopedcolor.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace onyx::ui
{
    ScopedImGuiColor::ScopedImGuiColor(onyxS32 colorId, onyxU32 color)
    {
        ImGui::PushStyleColor(colorId, color);
    }

    ScopedImGuiColor::ScopedImGuiColor(std::initializer_list<std::pair<onyxS32, onyxU32>> colorVars)
    {
        m_ColorVarsCount = static_cast<onyxS32>(colorVars.size());
        for (const auto& [colorId, color] : colorVars)
        {
            ImGui::PushStyleColor(colorId, color);
        }
    }

    ScopedImGuiColor::~ScopedImGuiColor()
    {
        Reset();
    }

    void ScopedImGuiColor::Reset()
    {
        ImGui::PopStyleColor(m_ColorVarsCount);
        m_ColorVarsCount = 0;
    }
}

#endif