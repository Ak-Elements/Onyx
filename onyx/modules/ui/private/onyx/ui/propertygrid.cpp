#include <onyx/ui/propertygrid.h>

#include <onyx/ui/modals/assetselectionmodal.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/ui/imguistyle.h>
#include <onyx/ui/widgets.h>

#if ONYX_IS_EDITOR

#include <imgui.h>
#include <imgui_internal.h>

namespace Onyx::Ui
{
    namespace Internal
    {
        Assets::AssetSystem* loc_AssetSystem = nullptr;
        Stack<ImGuiID> loc_PropertyGridIdStack;

        constexpr onyxU32 BACKGROUND_CHANNEL = 0;
        constexpr onyxU32 FOREGROUND_CHANNEL = 1;
    }

    onyxF32 PropertyGrid::ms_SplitterMinX = 0.0f;

    void PropertyGrid::SetAssetSystem(Assets::AssetSystem& assetSystem)
    {
        Internal::loc_AssetSystem = &assetSystem;
    }

    void PropertyGrid::BeginPropertyGrid(const StringView& propertyGrid, onyxF32 splitMinX)
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->ChannelsSplit(2);
        drawList->ChannelsSetCurrent(Internal::FOREGROUND_CHANNEL);

        ImGui::BeginGroup();

        Internal::loc_PropertyGridIdStack.push(ImGui::GetID(propertyGrid.data()));
        ImGuiID id = Internal::loc_PropertyGridIdStack.top();

        ImGui::PushID(id);
        ImGui::BeginGroup();

        String splitterIdStr(propertyGrid);
        splitterIdStr += "_splitterX";
        ImGuiID splitterPositionXId = ImGui::GetID(splitterIdStr.data());

        ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
        imguiStateStorage->SetInt(id, static_cast<onyxS32>(splitterPositionXId));
        ms_SplitterMinX = splitMinX;
    }

    void PropertyGrid::EndPropertyGrid()
    {
        ImGui::EndGroup();
        ImGui::PopID();

        DrawSplitter();

        ImGui::EndGroup();

        Internal::loc_PropertyGridIdStack.pop();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->ChannelsMerge();
    }

    void PropertyGrid::DrawPropertyName(const StringView& propertyName)
    {
        ImGuiID propertyGridID = Internal::loc_PropertyGridIdStack.top();

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
        
        ImGui::PushFont(ImGui::GetDefaultFont());

        // Render the text with ellipsis if it exceeds the available width
        ImGui::RenderTextEllipsis(
            ImGui::GetWindowDrawList(),
            label_pos,
            ImVec2(ellipsis_max, label_pos.y + label_size.y),
            ellipsis_max,
            ellipsis_max,
            label,
            label_end,
            &label_size
        );

        ImGui::PopFont();

        ImGui::Dummy(ImVec2(splitterPosX - indendation + style.DockingSeparatorSize + 2 * style.ItemInnerSpacing.x, ImGui::GetFrameHeightWithSpacing()));
    }

    void PropertyGrid::DrawPropertyValue(const InplaceFunction<void()>& functor)
    {
        functor();
        ImGui::EndHorizontal();
    }

    bool PropertyGrid::BeginPropertyGroup(const StringView& propertyName)
    {
        ImGui::PushID(propertyName.data());

        DrawPropertyName(propertyName);
        ImGui::EndHorizontal();

        ImGui::Indent();
        ImGui::BeginGroup();

        return true;
    }

    bool PropertyGrid::BeginCollapsiblePropertyGroup(const StringView& propertyName, ImGuiTreeNodeFlags flags)
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

    bool PropertyGrid::BeginCollapsiblePropertyGroup(const StringView& propertyName, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags)
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

    void PropertyGrid::EndPropertyGroup()
    {
        ImGui::EndGroup();
        ImGui::Unindent();

        ImGui::PopID();
    }

    bool PropertyGrid::DrawStringProperty(const StringView& propertyName, String& value)
    {
        return DrawStringProperty(propertyName, value, ImGuiInputTextFlags_None);
    }

    bool PropertyGrid::DrawStringProperty(const StringView& propertyName, String& value, ImGuiInputTextFlags textFlags)
    {
        DrawPropertyName(propertyName);

        bool hasModified = DrawStringInput(propertyName, value, ImVec2(0,0), textFlags);

        ImGui::EndHorizontal();

        return hasModified;
    }

    bool PropertyGrid::DrawAssetSelector(const StringView& propertyName, Assets::AssetId& outAssetId, Assets::AssetType assetType)
    {
        ONYX_ASSERT(Internal::loc_AssetSystem != nullptr);

        DrawPropertyName(propertyName);

        ImGui::PushID(propertyName.data());

        bool hasModified = AssetSelectionControl::Render(*Internal::loc_AssetSystem, assetType, outAssetId);

        ImGui::PopID();
        ImGui::EndHorizontal();

        return hasModified;
    }

    bool PropertyGrid::DrawBoolProperty(const StringView& propertyName, bool& value)
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

    bool PropertyGrid::DrawColorProperty(const StringView& propertyName, Vector3f& inOutColor)
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

    bool PropertyGrid::DrawColorProperty(const StringView& propertyName, Vector4f& inOutColor)
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

    bool PropertyGrid::DrawColorProperty(const StringView& propertyName, Vector4u8& inOutColor)
    {
        Vector4f color{ numeric_cast<onyxF32>(inOutColor[0]) / 255.0f, numeric_cast<onyxF32>(inOutColor[1]) / 255.0f, numeric_cast<onyxF32>(inOutColor[2]) / 255.0f, numeric_cast<onyxF32>(inOutColor[3]) / 255.0f };
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

    void PropertyGrid::DrawSplitter()
    {
        ImGuiID propertyGridID = Internal::loc_PropertyGridIdStack.top();

        ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
        onyxU32 splitterId = imguiStateStorage->GetInt(propertyGridID);
        float& storedSplitterPosX = *imguiStateStorage->GetFloatRef(splitterId, ms_SplitterMinX);

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
        drawList->ChannelsSetCurrent(Internal::BACKGROUND_CHANNEL);

        const onyxU32 splitterColor = isHeld ? ImGui::GetColorU32(ImGuiCol_SeparatorActive) : (isHovered ? ImGui::GetColorU32(ImGuiCol_SeparatorHovered) : ImGui::GetColorU32(ImGuiCol_Separator));
        drawList->AddRectFilled(visualBB.Min, visualBB.Max, splitterColor);

        drawList->ChannelsSetCurrent(Internal::FOREGROUND_CHANNEL);
        // Handle dragging the splitter
        if (isHeld)
        {
            // Update splitter position as the user drags it
            storedSplitterPosX = std::max(storedSplitterPosX + ImGui::GetIO().MouseDelta.x, ms_SplitterMinX);
        }

        // Change cursor on hover to indicate it's resizable
        if (isHovered)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }
    }
}
#endif