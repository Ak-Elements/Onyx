#pragma once

#if ONYX_USE_IMGUI

#include <variant>
#include <imgui.h>

namespace Onyx::Ui
{
    struct ScopedImGuiStyle
    {
    private:
        using StyleVarValue = std::variant<onyxF32, ImVec2>;  // To handle float or ImVec2 values

    public:
        ScopedImGuiStyle(ImGuiStyleVar styleVar, onyxF32 value)
        {
            ImGui::PushStyleVar(styleVar, value);
        }

        ScopedImGuiStyle(ImGuiStyleVar styleVar, const ImVec2& value)
        {
            ImGui::PushStyleVar(styleVar, value);
        }

        // list of style vars
        ScopedImGuiStyle(std::initializer_list<std::pair<ImGuiStyleVar, StyleVarValue>> styleVars)
        {
            m_StyleVarsCount = static_cast<onyxS32>(styleVars.size());
            for (const auto& [styleEnum, value] : styleVars)
            {
                // Push the appropriate type based on the value type (float or ImVec2)
                if (std::holds_alternative<onyxF32>(value)) {
                    ImGui::PushStyleVar(styleEnum, std::get<onyxF32>(value));
                }
                else if (std::holds_alternative<ImVec2>(value))
                {
                    ImGui::PushStyleVar(styleEnum, std::get<ImVec2>(value));
                }
            }
        }

        ~ScopedImGuiStyle()
        {
            Reset();
        }

        void Reset()
        {
            ImGui::PopStyleVar(m_StyleVarsCount);
            m_StyleVarsCount = 0;
        }

    private:
        onyxS32 m_StyleVarsCount = 1;
    };

    struct ScopedImGuiColor
    {
    public:
        ScopedImGuiColor(ImGuiCol colorId, onyxU32 color)
        {
            ImGui::PushStyleColor(colorId, color);
        }

        // list of color vars
        ScopedImGuiColor(std::initializer_list<std::pair<ImGuiCol, onyxU32>> colorVars)
        {
            m_ColorVarsCount = static_cast<onyxS32>(colorVars.size());
            for (const auto& [colorId, color] : colorVars)
            {
                ImGui::PushStyleColor(colorId, color);
            }
        }

        ~ScopedImGuiColor()
        {
            Reset();
        }

        void Reset()
        {
            ImGui::PopStyleColor(m_ColorVarsCount);
            m_ColorVarsCount = 0;
        }

    private:
        onyxS32 m_ColorVarsCount = 1;
    };

    struct ScopedImGuiId
    {
        ScopedImGuiId(StringView str)
        {
            ImGui::PushID(str.data());
        }

        ScopedImGuiId(onyxS32 id)
        {
            ImGui::PushID(id);
        }

        ~ScopedImGuiId()
        {
            ImGui::PopID();
        }
    };

    struct ScopedImGuiIndent
    {
        ScopedImGuiIndent()
        {
            ImGui::Indent(m_Indent);
        }

        ScopedImGuiIndent(onyxF32 indent)
            : m_Indent(indent)
        {
            ImGui::Indent(m_Indent);
        }

        ~ScopedImGuiIndent()
        {
            ImGui::Unindent(m_Indent);
        }

    private:
        onyxF32 m_Indent = 0.0f;
    };
}
#endif