#include <onyx/ui/widgets.h>

#if ONYX_USE_IMGUI

#include <onyx/ui/imguistyle.h>

#include <imgui.h>
#include <imgui_stacklayout.h>
#include <imgui_internal.h>

namespace Onyx::Ui
{
    void DrawItemBackground(onyxF32 rounding, onyxF32 borderThickness, onyxU32 color)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        //ImGuiContext& g = *GImGui;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();

        onyxF32 spacing = 0.0f;
        rectMin.x += borderThickness + spacing;
        rectMin.y += borderThickness + spacing;
        rectMax.x -= borderThickness + spacing;
        rectMax.y -= borderThickness + spacing;

        const ImRect rect{ rectMin, rectMax };

        // Draw background
        draw_list->AddRectFilled(rect.Min, rect.Max, color, rounding);
    }

    void DrawItemDropShadow(onyxF32 shadowOffset, onyxF32 rounding, onyxU32 color)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 rectMin = ImGui::GetItemRectMin();
        const ImVec2 rectMax = ImGui::GetItemRectMax();

        // Top shadow
        //draw_list->AddRectFilled(ImVec2(itemMin.x, itemMin.y - shadowOffset),
        //    ImVec2(itemMax.x, itemMin.y),
 
        draw_list->AddRectFilled(ImVec2(rectMin.x + shadowOffset, rectMin.y + shadowOffset),
            ImVec2(rectMax.x + shadowOffset, rectMax.y + shadowOffset),
            color, rounding);
    }

    void DrawItemBorder(onyxF32 thickness, onyxF32 rounding, onyxU32 color)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        //ImGuiContext& g = *GImGui;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();

        const ImRect rect{ rectMin , rectMax};

        // Draw border
        draw_list->AddRect(rect.Min, rect.Max, color, rounding, ImDrawFlags_None, thickness);
    }

    bool DrawSearchBar(String& searchString, const StringView& hintLabel, bool& grabFocus)
    {
        bool modified = false;
        // Unique identifier for the input text field
        static onyxS32 searchBarId = 0;
        const onyxF32 spacingX = 4.0f;
        const onyxF32 borderSize = 1.0f;
        const onyxF32 dropShadowSize = 2.0f;
        ImGuiID searchInputId;
        // Push a unique ID for the search bar
        ImGui::PushID(searchBarId++);

        // Adjust the size to account for border and drop shadow
        ImVec2 searchBarSize = ImVec2(ImGui::GetContentRegionAvail().x - dropShadowSize, ImGui::GetTextLineHeightWithSpacing() + 2 * borderSize);

        ImVec2 cursorPos = ImGui::GetCursorPos();

        // Adjust the size to account for border and drop shadow
        ImGui::SetNextItemAllowOverlap();
        if (ImGui::InvisibleButton("##searchBarInteraction", ImVec2(searchBarSize.x, searchBarSize.y)))
        {
            grabFocus = true;
        }
        ImGui::SetCursorPos(cursorPos);

        DrawItemBackground(3.0f, borderSize, 0xFF333333);

        ImGui::BeginHorizontal("##searchBar", searchBarSize);

        const onyxF32 framePaddingY = ImGui::GetStyle().FramePadding.y;

        // Search icon
        const onyxF32 iconSize = ImGui::GetTextLineHeightWithSpacing() - framePaddingY / 2.0f;
        const onyxF32 halfIconSize = iconSize / 2.0f;

        DrawSearchIcon(ImGui::GetWindowDrawList(), ImVec2(0, 0), halfIconSize, 0x33FFFFFF);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacingX);

        // Create the search bar input text field
        searchString.reserve(256);
        {
            ScopedImGuiColor scopedColors
            {
                { ImGuiCol_FrameBg, 0x00000000 },
                { ImGuiCol_FrameBgHovered, 0x00000000 },
                { ImGuiCol_FrameBgActive, 0x00000000 },
                { ImGuiCol_Border, 0x00000000 }
            };

            onyxS32 capacity = static_cast<onyxS32>(searchString.capacity());
            ImGui::SetNextItemWidth(searchBarSize.x - 2 * iconSize - 2 * spacingX);
            searchInputId = ImGui::GetID("##searchbarinput");
            if (ImGui::InputTextWithHint("##searchbarinput", hintLabel.data(), searchString.data(), capacity))
            {
                // Resize the string if needed
                searchString.resize(std::strlen(searchString.data()));
                modified = true;
            }
        }

        // Set the focus on the search bar if requested
        if (grabFocus)
        {
            if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
                && !ImGui::IsAnyItemActive()
                && !ImGui::IsMouseClicked(0))
            {
                ImGui::SetKeyboardFocusHere(-1);
            }

            if (ImGui::IsItemFocused())
                grabFocus = false;
        }

        ImGui::Spring();

        bool isSearching = searchString.empty() == false;
        if (isSearching)
        {
            if (DrawCloseButton(ImGui::GetWindowDrawList(), ImVec2(0.0f, 0.0f), halfIconSize, 0xC8A0A0A0))
            {
                searchString.clear();
                grabFocus = true; // grab focus in next update
                modified = true;
            }
        }

        ImGui::Spring(-1.0f, spacingX * 2.0f);

        ImGui::EndHorizontal();

        DrawItemBorder(borderSize, 3.0f, searchInputId == ImGui::GetActiveID() ? 0xFFFF7929 : 0x80474747);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);

        // Pop the ID
        ImGui::PopID();
        --searchBarId;

        return modified;
    }

    // move to a UI base class
    bool ContextMenuHeader(StringView label, ImGuiTreeNodeFlags flags)
    {
        // Calculate header area including spacing
        ImGuiStyle& currentStyle = ImGui::GetStyle();

        const ImVec2 cursorStart = ImGui::GetCursorScreenPos();
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        const ImVec2 headerAreaMin = cursorStart - ImVec2(0.0f, currentStyle.ItemSpacing.y);
        const ImVec2 headerAreaMax = cursorStart + ImVec2(contentRegion.x, ImGui::GetFrameHeight() + currentStyle.ItemSpacing.y);

        // Fill the background of the header (including gaps)
        const ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImGui::GetWindowDrawList()->AddRectFilled(headerAreaMin, headerAreaMax, bgColor);

        // Render the collapsible header
        const ImGuiID collapsibleId = ImGui::GetID(label.data());
        bool isOpen = ImGui::TreeNodeBehavior(collapsibleId, flags | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog, label.data());

        if (isOpen)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
            const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding))
                ? currentStyle.FramePadding
                : ImVec2(currentStyle.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, currentStyle.FramePadding.y));

            // Use bounding box of the last drawn item (the collapsing header)
            const ImVec2 header_min = ImGui::GetItemRectMin(); // Top-left of the header
            const ImVec2 header_max = ImGui::GetItemRectMax(); // Bottom-right of the header

            // Calculate separator line positions
            const ImVec2 label_size = ImGui::CalcTextSize(label.data());
            const float text_offset_x = header_min.x + ImGui::GetFontSize() + padding.x * 4 + label_size.x;
            const float line_y = header_min.y + ImGui::GetFrameHeight() * 0.5f; // Slightly below the header
            const ImVec2 start = ImVec2(text_offset_x, line_y);
            const ImVec2 end = ImVec2(header_max.x - padding.x, line_y);

            window->DrawList->AddLine(start, end, ImGui::GetColorU32(ImGuiCol_Separator));

            // next line
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetFrameHeight()));

        return isOpen;
    }

    bool ContextMenuHeader(StringView label, const InplaceFunction<bool()>& customHeader, ImGuiTreeNodeFlags flags)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiStyle& currentStyle = ImGui::GetStyle();

        // Start the horizontal layout
        ImGui::BeginGroup(); // Group to keep header and buttons together
        //

        // Calculate header area including spacing
        const ImVec2 cursorStart = ImGui::GetCursorScreenPos();
        const ImVec2 contentRegion = ImGui::GetContentRegionMax();
        const ImVec2 headerAreaMin = cursorStart - ImVec2(0.0f, currentStyle.ItemSpacing.y);
        const ImVec2 headerAreaMax = cursorStart + ImVec2(contentRegion.x, ImGui::GetFrameHeight() + currentStyle.ItemSpacing.y);

        // Fill the background of the header (including gaps)
        const ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
        ImGui::GetWindowDrawList()->AddRectFilled(headerAreaMin, headerAreaMax, bgColor);

        // Render the collapsible header
        const ImGuiID collapsibleId = ImGui::GetID(label.data());
        bool isOpen = ImGui::TreeNodeBehavior(collapsibleId, flags | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_NoAutoOpenOnLog, label.data());

        // Position the buttons on the same line as the header
        {
            ScopedImGuiId headerContentId(ImGui::GetIDWithSeed(collapsibleId, 0));
            ImGui::SameLine(); // Keep buttons aligned horizontally
            ImGui::BeginHorizontal("##customHeaderContent", ImVec2(ImGui::GetContentRegionAvail().x,0.0f));
            bool shouldOpen = customHeader();
            ImGui::EndHorizontal();

            if (shouldOpen && (isOpen == false))
            {
                ImGui::TreeNodeSetOpen(collapsibleId, shouldOpen);
                isOpen = true;
            }
            
            ImGui::SameLine();
            ImGui::SetCursorPosX(0.0f);
            const ImGuiID inputBlockerId = ImGui::GetID("##inputBlock");
            ImGui::InvisibleButton("##inputBlock", ImVec2(ImGui::GetContentRegionMax().x, ImGui::GetFrameHeight()));

            if (inputBlockerId == ImGui::GetHoveredID())
            {
                ImGui::SetHoveredID(collapsibleId);
            }
        }
        //ImGui::PopID();
        ImGui::EndGroup();

        return isOpen;
    }

    bool DrawStringInput(StringView id, String& value, const ImVec2& size, ImGuiInputTextFlags flags)
    {
        struct InputTextCallback_Payload
        {
            String* Str;
        };

        auto TextInputCallback = [](ImGuiInputTextCallbackData* data)
            {
                InputTextCallback_Payload* user_data = (InputTextCallback_Payload*)data->UserData;
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    // Resize string callback
                    // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
                    std::string* str = user_data->Str;
                    IM_ASSERT(data->Buf == str->c_str());
                    str->resize(data->BufTextLen);
                    data->Buf = (char*)str->c_str();
                }
                return 0;
            };

        ScopedImGuiId valueId(id);
        InputTextCallback_Payload payload;
        payload.Str = &value;

        return ImGui::InputTextEx("##inputText", nullptr, value.data(), static_cast<onyxS32>(value.capacity() + 1), size, flags | ImGuiInputTextFlags_CallbackResize, TextInputCallback, &payload);
    }

    void DrawMultilineText(StringView text, ImVec2 bounds, bool showEllipsis)
    {
        ONYX_UNUSED(text);
        ONYX_UNUSED(bounds);
        ONYX_UNUSED(showEllipsis);
        // Todo should be string views
        /*DynamicArray<String> lines = Split(text, '\n');

        const ImGuiStyle& style = ImGui::GetStyle();
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + bounds.x - style.FramePadding.x);

        onyxU32 maxLines = 0;
        if (showEllipsis)
        {
            float accumulatedHeight = 0;
            for (const String& line : lines)
            {
                ImVec2 lineSize = ImGui::CalcTextSize(line.data(), nullptr, false, bounds.x);

                bool exceedsHeight = (ImGui::GetCursorPosY() + lineSize.y) >= (bounds.y - style.FramePadding.y);
                if (showEllipsis && exceedsHeight)
                {
                    lines[i] += "...";
                    ImGui::TextUnformatted(lines[i].data());
                }
                else
                {
                    ImGui::TextUnformatted(lines[i].data());
                }


                accumulatedHeight += lineSize.y;
                ++maxLines;
            }

            showEllipsis &= lines.size() != maxLines;
            for (onyxU32 i = 0; i < maxLines; ++i)
            {
                
            }
        }
        else
        {
            ImGui::Text(text.data());
        }*/
        //ImGui::PopTextWrapPos();
    }

    bool DrawRenameInput(StringView id, String& outName, const ImVec2& size, bool& isSelected)
    {
        ScopedImGuiId scopedId(id);
        ImGuiStorage* stateStorage = ImGui::GetStateStorage();

        ImGuiID stateId = ImGui::GetID("state");
        bool isRenaming = stateStorage->GetBool(stateId, false);
        bool hasRenamed = false;
        if (isRenaming)
        {
            if (DrawStringInput("##renameInput", outName, size, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft))
            {
                isRenaming = false;
                hasRenamed = true;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) || (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()))
            {
                isRenaming = false;
            }
            else
            {
                ImGui::SetKeyboardFocusHere(-1);
            }
        }
        else
        {
            if (ImGui::Selectable(Format::Format("{}##renameInput", outName.data()), isSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowOverlap))
            {
                isSelected = true;
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    isRenaming = true;
                }
            }
        }

        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Rename"))
            {
                isRenaming = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        stateStorage->SetBool(stateId, isRenaming);
        return hasRenamed;
    }

    void DrawInfoIcon(ImDrawList* draw_list, ImVec2 offset, onyxF32 radius, onyxU32 color)
    {
        // Draw the surrounding circle
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 center = ImVec2(cursorPos.x + offset.x + radius, cursorPos.y + offset.y + radius);

        draw_list->AddCircle(center, radius, color, 0, 2.0f);

        // Draw the "i" (line and dot)
        onyxF32 line_height = radius * 0.5f;
        onyxF32 line_width = radius * 0.2f;

        ImVec2 rectStart = ImVec2(center.x - (line_width / 2.0f), center.y - line_height * 0.3f);
        ImVec2 rectEnd = ImVec2(center.x + (line_width / 2.0f), center.y + line_height * 0.8f);

        draw_list->AddRectFilled(rectStart, rectEnd, color, line_width);

        // Draw the dot
        ImVec2 dot_center = ImVec2(center.x, center.y - line_height * 0.8f);
        draw_list->AddCircleFilled(dot_center, line_width / 2.0f, color);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x + 2 * radius);
    }

    void DrawSearchIcon(ImDrawList* draw_list, ImVec2 offset, onyxF32 radius, onyxU32 color)
    {
        // Calculate the initial center position for the circle (lens of the magnifying glass)
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 circleCenter = ImVec2(cursorPos.x + offset.x + radius, cursorPos.y + offset.y + radius);

        // Define a smaller radius for the magnifying glass lens
        onyxF32 lens_radius = radius * 0.5f; // Adjust the size of the circle

        // Calculate the offset for top-left adjustment
        onyxF32 offset_x = radius * 0.1f; // Adjust this value to change the offset amount horizontally
        onyxF32 offset_y = radius * 0.1f; // Adjust this value to change the offset amount vertically

        // Adjust the circle center to account for the offset
        ImVec2 adjustedCenter = ImVec2(circleCenter.x - offset_x, circleCenter.y - offset_y);

        // Draw the circle (lens of the magnifying glass)
        draw_list->AddCircle(adjustedCenter, lens_radius, color, 0, 2.0f);

        // Draw the handle of the magnifying glass
        onyxF32 handle_length = radius * 0.3f;
        onyxF32 handle_width = radius * 0.2f;

        // Define the handle position relative to the adjusted circle center
        ImVec2 handle_start = ImVec2(adjustedCenter.x + lens_radius * 0.7f, adjustedCenter.y + lens_radius * 0.7f);
        ImVec2 handle_end = ImVec2(handle_start.x + handle_length, handle_start.y + handle_length);

        draw_list->AddLine(handle_start, handle_end, color, handle_width);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x + 2 * radius);
    }

    bool DrawCloseButton(ImDrawList* draw_list, ImVec2 offset, onyxF32 size, onyxU32 color)
    {
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        onyxF32 halfSize = size / 2.0f;
        ImVec2 center = ImVec2(cursorPos.x + offset.x + halfSize, cursorPos.y + offset.y + halfSize);

        ImGui::PushID("uniqueCloseButton");

        // Create a button
        bool pressed = ImGui::InvisibleButton("##close_button", ImVec2(size, size));

        if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()))
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        // Define the size of the "X" lines
        onyxF32 line_thickness = size * 0.2f; // Thickness of the lines
        onyxF32 line_length = size * 1.2f;    // Length of the lines

        // Calculate the end points for the diagonal lines
        ImVec2 line1_start = ImVec2(center.x - line_length * 0.5f, center.y - line_length * 0.5f);
        ImVec2 line1_end = ImVec2(center.x + line_length * 0.5f, center.y + line_length * 0.5f);
        ImVec2 line2_start = ImVec2(center.x - line_length * 0.5f, center.y + line_length * 0.5f);
        ImVec2 line2_end = ImVec2(center.x + line_length * 0.5f, center.y - line_length * 0.5f);

        // Draw the diagonal lines
        draw_list->AddLine(line1_start, line1_end, color, line_thickness);
        draw_list->AddLine(line2_start, line2_end, color, line_thickness);

        ImGui::PopID();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x + size);

        return pressed;
    }

    void DrawFolderIcon(ImDrawList* draw_list, ImVec2 offset, onyxF32 size, onyxF32 rounding, onyxU32 color_folder, onyxU32 color_folder_lid)
    {
        onyxF32 verticalSize = size * 0.75f;
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();

        ImVec2 halfSize(size * 0.5f, verticalSize * 0.5f);
        onyxF32 tab_height = verticalSize * 0.2f;  // Height of the top section
        onyxF32 tab_width = size * 0.3f;   // Width of the top section
        onyxF32 open_angle = verticalSize * 0.1f; // Opening effect
        ImVec2 center = ImVec2(cursorPos.x + offset.x + halfSize.x, cursorPos.y + offset.y + halfSize.x);

        ImVec2 top_left = center - halfSize;
        ImVec2 bottom_right = center + halfSize;
        // Draw folder body
        draw_list->AddRectFilled(top_left + ImVec2(0, open_angle), bottom_right, color_folder, rounding, ImDrawFlags_RoundCornersBottom | ImDrawFlags_RoundCornersTopRight);

        // Draw the top section
        ImVec2 lid_points[5] = {
        top_left,                  // Top-left of the lid
        top_left + ImVec2(tab_width, 0.0f),
        top_left + ImVec2(size * 0.5f, open_angle),
        top_left + ImVec2(tab_width, tab_height),
        top_left + ImVec2(0, tab_height)
        };

        draw_list->PathClear();

        draw_list->PathArcToFast(lid_points[0] + ImVec2(rounding, rounding), rounding, 6, 9);
        draw_list->PathLineTo(lid_points[1]);
        
        draw_list->PathLineTo(lid_points[2]);
        draw_list->PathLineTo(lid_points[3]);
        draw_list->PathLineTo(lid_points[4]);
        draw_list->PathLineTo(lid_points[0]);
        draw_list->PathFillConvex(color_folder_lid);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x + size);
    }
}
#endif