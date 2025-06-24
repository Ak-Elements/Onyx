#include <onyx/ui/propertygrid.h>

#include <onyx/ui/modals/assetselectionmodal.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/ui/imguistyle.h>
#include <onyx/ui/widgets.h>

#if ONYX_IS_EDITOR

#include <imgui.h>
#include <imgui_internal.h>

namespace Onyx::Ui::PropertyGrid
{
    namespace
    {
        Assets::AssetSystem* loc_AssetSystem = nullptr;
        Stack<ImGuiID> loc_PropertyGridIdStack;

        constexpr onyxU32 BACKGROUND_CHANNEL = 0;
        constexpr onyxU32 FOREGROUND_CHANNEL = 1;

        onyxF32 loc_SplitterMinX;

        void DrawSplitter()
        {
            ImGuiID propertyGridID = loc_PropertyGridIdStack.top();

            ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
            onyxU32 splitterId = imguiStateStorage->GetInt(propertyGridID);
            float& storedSplitterPosX = *imguiStateStorage->GetFloatRef(splitterId, loc_SplitterMinX);

            const ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 size = ImGui::GetItemRectSize();
            const float lineWidth = style.DockingSeparatorSize;
            ImVec2 screenPos = ImGui::GetCursorScreenPos();

            ImRect visualBB;
            visualBB.Min = ImVec2(screenPos.x + storedSplitterPosX - lineWidth, screenPos.y - size.y - style.FramePadding.y);
            visualBB.Max = ImVec2(screenPos.x + storedSplitterPosX, screenPos.y - style.FramePadding.y);

            ImRect interactionBB = visualBB;
            interactionBB.Min.x -= 1.0f;
            interactionBB.Max.x += 1.0f;

            bool isHovered = false;
            bool isHeld = false;
            ImGuiID splitterButtonId = ImGui::GetID("##splitter");
            ImGui::ItemAdd(visualBB, splitterButtonId, nullptr, ImGuiItemFlags_NoNav);
            ImGui::ButtonBehavior(interactionBB, splitterButtonId, &isHovered, &isHeld, ImGuiButtonFlags_FlattenChildren);

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->ChannelsSetCurrent(BACKGROUND_CHANNEL);

            const onyxU32 splitterColor = isHeld ? ImGui::GetColorU32(ImGuiCol_SeparatorActive) : (isHovered ? ImGui::GetColorU32(ImGuiCol_SeparatorHovered) : ImGui::GetColorU32(ImGuiCol_Separator));
            drawList->AddRectFilled(visualBB.Min, visualBB.Max, splitterColor);

            drawList->ChannelsSetCurrent(FOREGROUND_CHANNEL);
            // Handle dragging the splitter
            if (isHeld)
            {
                // Update splitter position as the user drags it
                storedSplitterPosX = std::max(storedSplitterPosX + ImGui::GetIO().MouseDelta.x, loc_SplitterMinX);
            }

            // Change cursor on hover to indicate it's resizable
            if (isHovered)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
        }
    }

    void SetAssetSystem(Assets::AssetSystem& assetSystem)
    {
        loc_AssetSystem = &assetSystem;
    }

    void BeginPropertyGrid(StringView propertyGrid, onyxF32 splitMinX)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->ChannelsSplit(2);
        drawList->ChannelsSetCurrent(FOREGROUND_CHANNEL);

        ImGui::BeginGroup();

        loc_PropertyGridIdStack.push(ImGui::GetID(propertyGrid.data()));
        ImGuiID id = loc_PropertyGridIdStack.top();

        ImGui::PushID(id);
        ImGui::BeginGroup();

        String splitterIdStr(propertyGrid);
        splitterIdStr += "_splitterX";
        ImGuiID splitterPositionXId = ImGui::GetID(splitterIdStr.data());

        ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
        imguiStateStorage->SetInt(id, static_cast<onyxS32>(splitterPositionXId));
        loc_SplitterMinX = splitMinX;
    }

    void EndPropertyGrid()
    {
        ImGui::EndGroup();
        ImGui::PopID();

        DrawSplitter();

        ImGui::EndGroup();

        loc_PropertyGridIdStack.pop();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->ChannelsMerge();
    }

    void DrawPropertyName(StringView propertyName)
    {
        DrawPropertyName(propertyName, "");
    }

    void DrawPropertyName(StringView propertyName, StringView tooltip)
    {
        ImGuiID propertyGridID = loc_PropertyGridIdStack.top();

        ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
        onyxU32 splitterId = imguiStateStorage->GetInt(propertyGridID);
        onyxF32 splitterPosX = imguiStateStorage->GetFloat(splitterId);

        const ImGuiStyle& style = ImGui::GetStyle();
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImGui::BeginHorizontal(propertyName.data());

        const char* label = propertyName.data();
        const char* label_end = ImGui::FindRenderedTextEnd(label);
        ImVec2 label_size = ImGui::CalcTextSize(label, label_end, true);
        ImVec2 label_pos = window->DC.CursorPos;
        onyxF32 indendation = window->DC.Indent.x;

        // Determine the height of the cell
        float cellHeight = ImGui::GetTextLineHeightWithSpacing();

        // Calculate vertical alignment offset to center the text
        float verticalOffset = (cellHeight - label_size.y) * 0.5f;

        // Adjust position for centered text
        label_pos.y += verticalOffset;

        onyxF32 ellipsis_max = label_pos.x + splitterPosX - style.ItemSpacing.x - indendation;
        ImVec2 label_pos_max = ImVec2(ellipsis_max, label_pos.y + label_size.y);
        ImGui::PushFont(ImGui::GetDefaultFont());

        // Render the text with ellipsis if it exceeds the available width
        ImGui::RenderTextEllipsis(
            ImGui::GetWindowDrawList(),
            label_pos,
            label_pos_max,
            ellipsis_max,
            ellipsis_max,
            label,
            label_end,
            &label_size
        );

        ImGui::PopFont();

        // move into name group
        const bool hasTooltip = tooltip.empty() == false;
        if (hasTooltip)
        {
            auto cursorPos = ImGui::GetCursorPos();
            ImGui::PushClipRect(label_pos, label_pos_max, true);
            DrawInfoIcon(ImGui::GetWindowDrawList(), ImVec2(label_size.x + style.ItemSpacing.x, verticalOffset), ImGui::GetTextLineHeight() / 2.0f, 0x33FFFFFF);
            ImGui::PopClipRect();
            ImGui::SetCursorPos(cursorPos);
        }

        ImGui::Dummy(ImVec2(splitterPosX - indendation + style.DockingSeparatorSize + 2 * style.ItemInnerSpacing.x, ImGui::GetFrameHeightWithSpacing()));

        if (hasTooltip && ImGui::BeginItemTooltip())
        {
            ImGui::TextEx(tooltip.data());
            ImGui::EndTooltip();
        }
    }

    void DrawPropertyValue(const InplaceFunction<void(), 64>& functor)
    {
        functor();
        ImGui::EndHorizontal();
    }

    bool BeginPropertyGroup(StringView propertyName)
    {
        ImGui::PushID(propertyName.data());

        DrawPropertyName(propertyName);
        ImGui::EndHorizontal();

        ImGui::Indent();
        ImGui::BeginGroup();

        return true;
    }

    bool BeginCollapsiblePropertyGroup(StringView propertyName, ImGuiTreeNodeFlags flags)
    {
        ImGui::PushID(propertyName.data());

        ScopedImGuiStyle style
        {
            { ImGuiStyleVar_FrameBorderSize, 0.0f },
        };

        if (ContextMenuHeader(propertyName.data(), flags | ImGuiTreeNodeFlags_Framed))
        {
            ImGui::Indent();
            ImGui::BeginGroup();
            return true;
        }
        else
        {
            ImGui::PopID();
            return false;
        }
    }

    bool BeginCollapsiblePropertyGroup(StringView propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags)
    {
        ImGui::PushID(propertyName.data());

        ScopedImGuiStyle style
        {
            { ImGuiStyleVar_FrameBorderSize, 0.0f },
        };

        if (ContextMenuHeader(propertyName.data(), customHeader, flags | ImGuiTreeNodeFlags_Framed))
        {
            ImGui::Indent();
            ImGui::BeginGroup();
            return true;
        }
        else
        {
            ImGui::PopID();
            return false;
        }
    }

    void EndPropertyGroup()
    {
        ImGui::EndGroup();
        ImGui::Unindent();

        ImGui::PopID();
    }

    bool DrawStringProperty(StringView propertyName, String& value)
    {
        return DrawStringProperty(propertyName, value, ImGuiInputTextFlags_None);
    }

    bool DrawStringProperty(StringView propertyName, String& value, ImGuiInputTextFlags textFlags)
    {
        DrawPropertyName(propertyName);

        bool hasModified = DrawStringInput(propertyName, value, ImVec2(0,0), textFlags);

        ImGui::EndHorizontal();

        return hasModified;
    }

    bool DrawAssetSelector(StringView propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType)
    {
        ONYX_ASSERT(loc_AssetSystem != nullptr);

        DrawPropertyName(propertyName);

        ImGui::PushID(propertyName.data());

        bool hasModified = AssetSelectionControl::Render(*loc_AssetSystem, assetType, outAssetId);

        ImGui::PopID();
        ImGui::EndHorizontal();

        return hasModified;
    }

    bool DrawBoolProperty(StringView propertyName, bool& value)
    {
        DrawPropertyName(propertyName);

        // Draw Value
        bool hasModified = false;
        {
            ScopedImGuiId valueId(propertyName);
            hasModified = ImGui::Checkbox("##inputBool", &value);
        }

        ImGui::EndHorizontal();

        return hasModified;
    }

    bool DrawColorProperty(StringView propertyName, Vector3f32& inOutColor)
    {
        DrawPropertyName(propertyName);
        ImGui::EndHorizontal();

        // Draw Value outside horizontal group as its not working with the layout
        ImGui::SameLine();
        bool hasModified = false;
        float color[3] = { inOutColor[0], inOutColor[1], inOutColor[2] };
        if (ImGui::ColorEdit3("##inputColor", color))
        {
            hasModified = true;
            inOutColor[0] = color[0];
            inOutColor[1] = color[1];
            inOutColor[2] = color[2];
        }

        return hasModified;
    }

    bool DrawColorProperty(StringView propertyName, Vector4f32& inOutColor)
    {
        DrawPropertyName(propertyName);
        ImGui::EndHorizontal();

        // Draw Value outside horizontal group as its not working with the layout
        ImGui::SameLine();
        bool hasModified = false;
        float color[4] = { inOutColor[0], inOutColor[1], inOutColor[2], inOutColor[3]};
        if (ImGui::ColorEdit4("##inputColor", color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            hasModified = true;
            inOutColor[0] = color[0];
            inOutColor[1] = color[1];
            inOutColor[2] = color[2];
            inOutColor[3] = color[3];
        }

        return hasModified;
    }

    bool DrawColorProperty(StringView propertyName, Vector4u8& inOutColor)
    {
        Vector4f32 color{ numeric_cast<onyxF32>(inOutColor[0]) / 255.0f, numeric_cast<onyxF32>(inOutColor[1]) / 255.0f, numeric_cast<onyxF32>(inOutColor[2]) / 255.0f, numeric_cast<onyxF32>(inOutColor[3]) / 255.0f };
        if (DrawColorProperty(propertyName, color))
        {
            inOutColor[0] = numeric_cast<onyxU8>(color[0] * 255.0f);
            inOutColor[1] = numeric_cast<onyxU8>(color[1] * 255.0f);
            inOutColor[2] = numeric_cast<onyxU8>(color[2] * 255.0f);
            inOutColor[3] = numeric_cast<onyxU8>(color[3] * 255.0f);
            return true;
        }

        return false;
    }
}
#endif