#include <onyx/ui/scopedstyle.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace Onyx::Ui
{
    ScopedImGuiStyle::ScopedImGuiStyle(onyxS32 styleVar, onyxF32 value)
    {
        ImGui::PushStyleVar(styleVar, value);
    }

    ScopedImGuiStyle::ScopedImGuiStyle(onyxS32 styleVar, const Vector2f32& value)
    {
        ImGui::PushStyleVar(styleVar, { value[0], value[1] });
    }

    ScopedImGuiStyle::ScopedImGuiStyle(onyxS32 styleVar, const ImVec2& value)
    {
        ImGui::PushStyleVar(styleVar, value );
    }

    ScopedImGuiStyle::ScopedImGuiStyle(std::initializer_list<std::pair<onyxS32, StyleVarValue>> styleVars)
    {
        m_StyleVarsCount = static_cast<onyxS32>(styleVars.size());
        for (const auto& [styleEnum, value] : styleVars)
        {
            // Push the appropriate type based on the value type (float or Vector2f)
            if (std::holds_alternative<onyxF32>(value))
            {
                ImGui::PushStyleVar(styleEnum, std::get<onyxF32>(value));
            }
            else if (std::holds_alternative<Vector2f32>(value))
            {
                const Vector2f32& styleValue = std::get<Vector2f32>(value);
                ImGui::PushStyleVar(styleEnum, { styleValue[0], styleValue[1] });
            }
            else if (std::holds_alternative<ImVec2>(value))
            {
                const ImVec2 styleValue = std::get<ImVec2>(value);
                ImGui::PushStyleVar(styleEnum, styleValue);
            }
        }
    }

    ScopedImGuiStyle::~ScopedImGuiStyle()
    {
        Reset();
    }

    void ScopedImGuiStyle::Reset()
    {
        ImGui::PopStyleVar(m_StyleVarsCount);
        m_StyleVarsCount = 0;
    }
}

#endif