#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>

#if ONYX_USE_IMGUI

#include <onyx/localization/localization.h>
#include <onyx/localization/localizedstring.h>

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace onyx::ui {

void drawCustomWindowTitleBar( StringView title, const InplaceFunction< void() >& customFunctor ) {
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    bool isFocused = ImGui::IsWindowFocused();

    const float titleBarH = ImGui::GetFrameHeight(); // matches ImGui's own title bar height
    const float padding = ImGui::GetStyle().FramePadding.x;

    ImVec2 barMin = windowPos;
    ImVec2 barMax = ImVec2( windowPos.x + windowSize.x, windowPos.y + titleBarH );

    // --- Background: active vs inactive, matches ImGui exactly ---
    ImU32 barColor = ImGui::GetColorU32( isFocused ? ImGuiCol_TitleBgActive : ImGuiCol_TitleBg );
    draw->AddRectFilled( barMin, barMax, barColor, ImGui::GetStyle().WindowRounding, ImDrawFlags_RoundCornersTop );

    // --- Title text ---
    ImVec2 textPos = ImVec2( barMin.x + titleBarH + padding, // offset past icon
                             barMin.y + ( titleBarH - ImGui::GetTextLineHeight() ) * 0.5f );
    draw->AddText( textPos, ImGui::GetColorU32( ImGuiCol_Text ), title.data() );

    customFunctor();

    // --- Drag region ---
    float dragStartX = padding + titleBarH;
    ImGui::SetCursorPos( ImVec2( dragStartX, 0 ) );
    ImGui::InvisibleButton( "##drag", ImVec2( windowSize.x - dragStartX - titleBarH, titleBarH ) );
    if( ImGui::IsItemActive() )
        ImGui::SetWindowPos(
            ImVec2( windowPos.x + ImGui::GetIO().MouseDelta.x, windowPos.y + ImGui::GetIO().MouseDelta.y ) );

    // --- Content area ---
    ImGui::SetCursorPos( ImVec2( padding, titleBarH + ImGui::GetStyle().WindowPadding.y ) );
}

void drawPinnableWindowTitleBar( StringView title, bool& isPinned ) {
    auto drawPinFunctor = [ & ]() {
        const float titleBarH = ImGui::GetFrameHeight(); // matches ImGui's own title bar height
        const float padding = ImGui::GetStyle().FramePadding.x;
        const float iconSize = std::round( titleBarH * 0.6f );
        ImU32 pinColor = ImGui::GetColorU32( isPinned ? ImGuiCol_CheckMark : ImGuiCol_TextDisabled );

        ImGui::SetCursorPos( ImVec2( padding, ( titleBarH - titleBarH * 0.8f ) * 0.5f ) );
        ImGui::InvisibleButton( "##pin", ImVec2( titleBarH, titleBarH * 0.8f ) );
        bool pinHovered = ImGui::IsItemHovered();
        bool pinClicked = ImGui::IsItemClicked();
        if( pinHovered )
            pinColor = ImGui::GetColorU32( ImGuiCol_Text ); // brighten on hover
        if( pinClicked )
            isPinned = !isPinned;
        if( pinHovered )
            ImGui::SetTooltip( isPinned ? "Unpin" : "Pin" );

        ImGui::SetCursorPos( ImVec2( padding, ( titleBarH - iconSize ) * 0.5f ) );
        drawPinIcon( ImGui::GetWindowDrawList(), ImVec2( 0, 0 ), iconSize, 0.0f, pinColor );
    };

    drawCustomWindowTitleBar( title, drawPinFunctor );
}

void drawItemBackground( float32 rounding, float32 borderThickness, uint32_t color ) {
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();
    drawItemBackground( rectMin, rectMax, rounding, borderThickness, color );
}

void drawItemBackground( ImVec2 rectMin, ImVec2 rectMax, float32 rounding, float32 borderThickness, uint32_t color ) {
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return;

    // ImGuiContext& g = *GImGui;
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float32 spacing = 0.0f;
    rectMin.x += borderThickness + spacing;
    rectMin.y += borderThickness + spacing;
    rectMax.x -= borderThickness + spacing;
    rectMax.y -= borderThickness + spacing;

    const ImRect rect{ rectMin, rectMax };

    // Draw background
    drawList->AddRectFilled( rect.Min, rect.Max, color, rounding );
}

void drawItemDropShadow( float32 shadowOffset, float32 rounding, uint32_t color ) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 rectMin = ImGui::GetItemRectMin();
    const ImVec2 rectMax = ImGui::GetItemRectMax();

    // Top shadow
    // draw_list->AddRectFilled(ImVec2(itemMin.x, itemMin.y - shadowOffset),
    //    ImVec2(itemMax.x, itemMin.y),

    drawList->AddRectFilled( ImVec2( rectMin.x + shadowOffset, rectMin.y + shadowOffset ),
                             ImVec2( rectMax.x + shadowOffset, rectMax.y + shadowOffset ),
                             color,
                             rounding );
}

void drawItemBorder( float32 thickness, float32 rounding, uint32_t color ) {
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return;

    // ImGuiContext& g = *GImGui;
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 rectMin = ImGui::GetItemRectMin();
    ImVec2 rectMax = ImGui::GetItemRectMax();

    const ImRect rect{ rectMin, rectMax };

    // Draw border
    drawList->AddRect( rect.Min, rect.Max, color, rounding, ImDrawFlags_None, thickness );
}

bool drawMultiSelect( StringView id, const DynamicArray< StringView >& items, HashSet< uint32_t >& selectedIndices ) {
    ScopedImGuiId scopedId( id );

    ImGuiStorage* stateStorage = ImGui::GetStateStorage();
    ImGuiID searchStringId = ImGui::GetID( "searchString" );
    bool shouldFocus = ImGui::IsWindowAppearing();

    String* searchStringPtr = static_cast< String* >( stateStorage->GetVoidPtr( searchStringId ) );
    if( searchStringPtr == nullptr ) {
        searchStringPtr = new String();
        stateStorage->SetVoidPtr( searchStringId, searchStringPtr );
    }

    String& searchString = *searchStringPtr;
    bool modified = false;
    drawSearchBar( searchString, "Search...", shouldFocus );

    constexpr ImGuiWindowFlags ListFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                                           ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                                           ImGuiWindowFlags_NoBackground;

    if( ImGui::BeginChild( "##ScrollList",
                           ImVec2( 0, ImGui::GetTextLineHeightWithSpacing() * 10 ),
                           ImGuiChildFlags_FrameStyle | ImGuiChildFlags_NavFlattened,
                           ListFlags ) ) {
        int32_t itemsCount = numericCast< int32_t >( items.size() );
        for( int32_t i = 0; i < itemsCount; ++i ) {
            StringView item = items[ i ];
            if( !searchString.empty() && ignoreCaseFind( item, searchString ) == StringView::npos )
                continue;

            ScopedImGuiId scopedItemId( i );
            bool isSelected = selectedIndices.contains( i );
            bool itemModified = false;

            // Full-width selectable behind the checkbox
            const ImVec2 rowStart = ImGui::GetCursorScreenPos();
            if( ImGui::Selectable( "##sel",
                                   isSelected,
                                   (uint32_t)ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_AllowOverlap |
                                       ImGuiSelectableFlags_SpanAllColumns,
                                   ImVec2( 0, ImGui::GetFrameHeight() ) ) ) {
                isSelected = !isSelected;
                itemModified = true;
            }

            // Overlay checkbox flush to the left of the row
            ImGui::SameLine( 0, 0 );
            ImGui::SetCursorScreenPos( rowStart );
            if( ImGui::Checkbox( "##chk", &isSelected ) )
                itemModified = true;

            // Label to the right of the checkbox
            ImGui::SameLine();
            ImGui::TextUnformatted( item.data() );

            if( itemModified ) {
                ImGui::ClearActiveID();

                if( isSelected )
                    selectedIndices.emplace( i );
                else
                    selectedIndices.erase( i );
                modified = true;
            }
        }
    }
    ImGui::EndChild();
    return modified;
}

bool drawSearchBar( String& searchString, StringView hintLabel, bool& grabFocus ) {
    bool modified = false;
    // Unique identifier for the input text field
    static int32_t SearchBarId = 0;
    const float32 spacingX = 4.0f;
    const float32 borderSize = 1.0f;
    const float32 dropShadowSize = 2.0f;
    ImGuiID searchInputId;
    // Push a unique ID for the search bar
    ImGui::PushID( SearchBarId++ );
    ImGui::BeginGroup();
    ImGui::SuspendLayout();
    // Adjust the size to account for border and drop shadow
    float32 width = ImGui::CalcItemWidth();
    ImVec2 searchBarSize = ImVec2( width - dropShadowSize, ImGui::GetTextLineHeightWithSpacing() + 2 * borderSize );

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    // Adjust the size to account for border and drop shadow
    ImGui::SetNextItemAllowOverlap();
    if( ImGui::InvisibleButton( "##searchBarInteraction", ImVec2( searchBarSize.x, searchBarSize.y ) ) ) {
        grabFocus = true;
    }
    ImGui::SetCursorScreenPos( cursorPos );

    drawItemBackground( cursorPos,
                        cursorPos + searchBarSize,
                        3.0f,
                        borderSize,
                        ImGui::GetColorU32( ImGuiCol_FrameBg ) );
    const float32 framePaddingY = ImGui::GetStyle().FramePadding.y;

    // Search icon
    const float32 iconSize = ImGui::GetTextLineHeightWithSpacing() - framePaddingY / 2.0f;
    const float32 halfIconSize = iconSize / 2.0f;

    drawSearchIcon( ImGui::GetWindowDrawList(),
                    ImVec2( 0, framePaddingY * 0.5f ),
                    halfIconSize,
                    ImGui::GetColorU32( ImGuiCol_TextDisabled ) );
    ImGui::SetCursorScreenPos( ImGui::GetCursorScreenPos() + ImVec2( spacingX, 0 ) );

    {
        ScopedImGuiColor scopedColors{ { ImGuiCol_FrameBg, 0x00000000 },
                                       { ImGuiCol_FrameBgHovered, 0x00000000 },
                                       { ImGuiCol_FrameBgActive, 0x00000000 },
                                       { ImGuiCol_Border, 0x00000000 } };

        // ImGui::SameLine();
        ImGui::SetNextItemWidth( searchBarSize.x - 2 * iconSize - 2 * spacingX );
        searchInputId = ImGui::GetID( "##searchbarinput" );
        if( ui::drawStringInput( "##searchbarinput", hintLabel, searchString ) ) {
            modified = true;
        }
    }

    // Set the focus on the search bar if requested
    if( grabFocus ) {
        if( ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows ) && !ImGui::IsAnyItemActive() &&
            !ImGui::IsMouseClicked( 0 ) ) {
            ImGui::SetKeyboardFocusHere( -1 );
        }

        if( ImGui::IsItemFocused() )
            grabFocus = false;
    }

    // ImGui::Spring();
    ImGui::SameLine();
    bool isSearching = searchString.empty() == false;
    if( isSearching ) {
        if( drawCloseButton( ImGui::GetWindowDrawList(),
                             ImVec2( 0.0f, ( iconSize - halfIconSize + framePaddingY ) * 0.5f ),
                             halfIconSize,
                             ImGui::GetColorU32( ImGuiCol_TextDisabled ) ) ) {
            searchString.clear();
            grabFocus = true; // grab focus in next update
            modified = true;
        }
    }

    ImGui::ResumeLayout();
    ImGui::EndGroup();
    bool isActive = searchInputId == ImGui::GetActiveID();
    drawItemBorder( borderSize,
                    3.0f,
                    isActive ? ImGui::GetColorU32( ImGuiCol_ButtonActive ) : ImGui::GetColorU32( ImGuiCol_Border ) );
    // Pop the ID
    ImGui::PopID();
    --SearchBarId;
    return modified;
}

bool contextMenuHeader( const localization::LocalizedString& label, ImGuiTreeNodeFlags flags ) {
    return contextMenuHeader( label.Get(), flags );
}

// move to a UI base class
bool contextMenuHeader( StringView label, ImGuiTreeNodeFlags flags ) {
    // Fill the background of the header (including gaps)
    // const ImU32 bgColor = ImGui::GetColorU32(ImGuiCol_FrameBg);
    // ImGui::GetWindowDrawList()->AddRectFilled(headerAreaMin, headerAreaMax, bgColor);

    // Render the collapsible header
    const ImGuiID collapsibleId = ImGui::GetID( label.data() );
    ScopedImGuiStyle style{ { ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) } };
    bool isOpen = ImGui::TreeNodeBehavior( collapsibleId,
                                           flags | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                               ImGuiTreeNodeFlags_NoAutoOpenOnLog,
                                           label.data() );

    if( isOpen ) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        //// Use bounding box of the last drawn item (the collapsing header)
        const ImVec2 headerMin = ImGui::GetItemRectMin(); // Top-left of the header
        const ImVec2 headerMax = ImGui::GetItemRectMax(); // Bottom-right of the header

        //// Calculate separator line positions
        const ImVec2 labelSize = ImGui::CalcTextSize( label.data() );
        float32 margin = ( ( headerMax.x - headerMin.x ) - labelSize.x );
        const float textOffsetX = headerMin.x + margin;
        const float lineY = headerMin.y + labelSize.y + 1; // Slightly below the header
        const ImVec2 start = ImVec2( textOffsetX, lineY );
        const ImVec2 end = ImVec2( ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x - margin, lineY );

        window->DrawList->AddLine( start, end, ImGui::GetColorU32( ImGuiCol_SeparatorActive ), 1 );

        //// next line
    }

    ImGui::SameLine();
    ImGui::Dummy( ImVec2( 0.0f, ImGui::GetFrameHeight() ) );

    return isOpen;
}

bool contextMenuHeader( StringView label, const InplaceFunction< bool() >& customHeader, ImGuiTreeNodeFlags flags ) {
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return false;

    ImGuiStyle& currentStyle = ImGui::GetStyle();

    // Start the horizontal layout
    ImGui::BeginGroup(); // Group to keep header and buttons together

    // Calculate header area including spacing
    const ImVec2 cursorStart = ImGui::GetCursorScreenPos();
    const ImVec2 contentRegion = ImGui::GetContentRegionMax();
    const ImVec2 headerAreaMin = cursorStart - ImVec2( 0.0f, currentStyle.ItemSpacing.y );
    const ImVec2 headerAreaMax = cursorStart +
                                 ImVec2( contentRegion.x, ImGui::GetFrameHeight() + currentStyle.ItemSpacing.y );

    // Fill the background of the header (including gaps)
    const ImU32 bgColor = ImGui::GetColorU32( ImGuiCol_FrameBg );
    ImGui::GetWindowDrawList()->AddRectFilled( headerAreaMin, headerAreaMax, bgColor );

    // Render the collapsible header
    const ImGuiID collapsibleId = ImGui::GetID( label.data() );
    bool isOpen = ImGui::TreeNodeBehavior( collapsibleId,
                                           flags | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                               ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_NoAutoOpenOnLog,
                                           label.data() );

    // Position the buttons on the same line as the header
    {
        ScopedImGuiId headerContentId( static_cast< int32_t >( ImGui::GetIDWithSeed( (int32_t)collapsibleId, 0 ) ) );
        ImGui::SameLine(); // Keep buttons aligned horizontally
        ImGui::BeginHorizontal( "##customHeaderContent", ImVec2( ImGui::GetContentRegionAvail().x, 0.0f ) );
        bool shouldOpen = customHeader();
        ImGui::EndHorizontal();

        if( shouldOpen && ( isOpen == false ) ) {
            ImGui::TreeNodeSetOpen( collapsibleId, shouldOpen );
            isOpen = true;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX( 0.0f );
        const ImGuiID inputBlockerId = ImGui::GetID( "##inputBlock" );
        ImGui::InvisibleButton( "##inputBlock", ImVec2( ImGui::GetContentRegionMax().x, ImGui::GetFrameHeight() ) );

        if( inputBlockerId == ImGui::GetHoveredID() ) {
            ImGui::SetHoveredID( collapsibleId );
        }
    }
    // ImGui::PopID();
    ImGui::EndGroup();

    return isOpen;
}

bool drawStringInput( StringView id, StringView hint, String& value ) {
    return drawStringInput( id, hint, value, ImVec2( 0, 0 ), ImGuiInputTextFlags_None );
}

bool drawStringInput( StringView id, String& value, const ImVec2& size, ImGuiInputTextFlags flags ) {
    return drawStringInput( id, "", value, size, flags );
}

bool drawStringInput( StringView id, StringView hint, String& value, const ImVec2& size, ImGuiInputTextFlags flags ) {
    struct InputTextCallbackPayload {
        String* Str;
    };

    auto textInputCallback = []( ImGuiInputTextCallbackData* data ) {
        InputTextCallbackPayload* userData = (InputTextCallbackPayload*)data->UserData;
        if( data->EventFlag == ImGuiInputTextFlags_CallbackResize ) {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them
            // back to what we want.
            std::string* str = userData->Str;
            IM_ASSERT( data->Buf == str->c_str() );
            str->resize( data->BufTextLen );
            data->Buf = (char*)str->c_str();
        }
        return 0;
    };

    // ScopedImGuiId valueId(id);
    InputTextCallbackPayload payload{};
    payload.Str = &value;

    return ImGui::InputTextEx( id.data(),
                               hint.data(),
                               value.data(),
                               static_cast< int32_t >( value.capacity() + 1 ),
                               size,
                               flags | ImGuiInputTextFlags_CallbackResize,
                               textInputCallback,
                               &payload );
}

bool drawStringInput( StringView id, StringView value, const ImVec2& size, ImGuiInputTextFlags flags ) {
    // const cast because we do not intend to edit anyways but imgui wants it to be a non const string
    return ImGui::InputTextEx( id.data(),
                               nullptr,
                               const_cast< char* >( value.data() ),
                               static_cast< int32_t >( value.length() ),
                               size,
                               flags | ImGuiInputTextFlags_ReadOnly );
}

bool drawRenameInput( StringView id, String& outName, const ImVec2& size, bool& isSelected ) {
    ScopedImGuiId scopedId( id );
    ImGuiStorage* stateStorage = ImGui::GetStateStorage();

    ImGuiID stateId = ImGui::GetID( "state" );
    bool isRenaming = stateStorage->GetBool( stateId, false );
    bool hasRenamed = false;
    if( isRenaming ) {
        if( drawStringInput( "##renameInput",
                             outName,
                             size,
                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft ) ) {
            isRenaming = false;
            hasRenamed = true;
        }

        bool hasLeftClickedOutside = ImGui::IsMouseClicked( ImGuiMouseButton_Left ) && !ImGui::IsItemHovered();
        if( ImGui::IsKeyPressed( ImGuiKey_Escape, false ) || hasLeftClickedOutside ) {
            isRenaming = false;
            hasRenamed = hasLeftClickedOutside;
        } else {
            ImGui::SetKeyboardFocusHere( -1 );
        }
    } else {
        if( ImGui::Selectable( format::format( "{}##renameInput", outName.data() ),
                               isSelected,
                               ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowOverlap ) ) {
            isSelected = true;
            if( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) ) {
                isRenaming = true;
            }
        }
    }

    if( ImGui::BeginPopupContextItem( nullptr, ImGuiPopupFlags_MouseButtonRight ) ) {
        if( ImGui::MenuItem( onyx::localization::generic::Rename.Get().data() ) ) {
            isRenaming = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    stateStorage->SetBool( stateId, isRenaming );
    return hasRenamed;
}

void drawInfoIcon( ImDrawList* drawList, ImVec2 offset, float32 radius, uint32_t color ) {
    // Draw the surrounding circle
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2( cursorPos.x + offset.x + radius, cursorPos.y + offset.y + radius );

    drawList->AddCircle( center, radius, color, 0, 2.0f );

    // Draw the "i" (line and dot)
    float32 lineHeight = radius * 0.5f;
    float32 lineWidth = radius * 0.2f;

    ImVec2 rectStart = ImVec2( center.x - ( lineWidth / 2.0f ), center.y - lineHeight * 0.3f );
    ImVec2 rectEnd = ImVec2( center.x + ( lineWidth / 2.0f ), center.y + lineHeight * 0.8f );

    drawList->AddRectFilled( rectStart, rectEnd, color, lineWidth );

    // Draw the dot
    ImVec2 dotCenter = ImVec2( center.x, center.y - lineHeight * 0.8f );
    drawList->AddCircleFilled( dotCenter, lineWidth / 2.0f, color );

    ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset.x + 2 * radius );
}

void drawSearchIcon( ImDrawList* drawList, ImVec2 offset, float32 radius, uint32_t color ) {
    // Calculate the initial center position for the circle (lens of the magnifying glass)
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 circleCenter = ImVec2( cursorPos.x + offset.x + radius, cursorPos.y + offset.y + radius );

    // Define a smaller radius for the magnifying glass lens
    float32 lensRadius = radius * 0.5f; // Adjust the size of the circle

    // Calculate the offset for top-left adjustment
    float32 offsetX = radius * 0.1f; // Adjust this value to change the offset amount horizontally
    float32 offsetY = radius * 0.1f; // Adjust this value to change the offset amount vertically

    // Adjust the circle center to account for the offset
    ImVec2 adjustedCenter = ImVec2( circleCenter.x - offsetX, circleCenter.y - offsetY );

    // Draw the circle (lens of the magnifying glass)
    drawList->AddCircle( adjustedCenter, lensRadius, color, 0, 2.0f );

    // Draw the handle of the magnifying glass
    float32 handleLength = radius * 0.3f;
    float32 handleWidth = radius * 0.2f;

    // Define the handle position relative to the adjusted circle center
    ImVec2 handleStart = ImVec2( adjustedCenter.x + lensRadius * 0.7f, adjustedCenter.y + lensRadius * 0.7f );
    ImVec2 handleEnd = ImVec2( handleStart.x + handleLength, handleStart.y + handleLength );

    drawList->AddLine( handleStart, handleEnd, color, handleWidth );

    ImGui::SetCursorScreenPos( cursorPos + ImVec2( offset.x + 2 * radius, 0 ) );
}

bool drawCloseButton( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    float32 halfSize = size / 2.0f;
    ImVec2 center = ImVec2( cursorPos.x + offset.x + halfSize, cursorPos.y + offset.y + halfSize );

    ImGui::PushID( "uniqueCloseButton" );

    // Create a button
    ImGui::SetCursorScreenPos( cursorPos + ImVec2( offset.x, offset.y ) );
    bool pressed = ImGui::InvisibleButton( "##close_button", ImVec2( size, size ) );
    ImGui::SetCursorScreenPos( cursorPos );

    if( ImGui::IsMouseHoveringRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() ) )
        ImGui::SetMouseCursor( ImGuiMouseCursor_Arrow );

    // Define the size of the "X" lines
    float32 lineThickness = size * 0.2f; // Thickness of the lines
    float32 lineLength = size * 1.2f;    // Length of the lines

    // Calculate the end points for the diagonal lines
    ImVec2 line1Start = ImVec2( center.x - lineLength * 0.5f, center.y - lineLength * 0.5f );
    ImVec2 line1End = ImVec2( center.x + lineLength * 0.5f, center.y + lineLength * 0.5f );
    ImVec2 line2Start = ImVec2( center.x - lineLength * 0.5f, center.y + lineLength * 0.5f );
    ImVec2 line2End = ImVec2( center.x + lineLength * 0.5f, center.y - lineLength * 0.5f );

    // Draw the diagonal lines
    drawList->AddLine( line1Start, line1End, color, lineThickness );
    drawList->AddLine( line2Start, line2End, color, lineThickness );

    ImGui::PopID();

    ImGui::SetCursorScreenPos( cursorPos + ImVec2( offset.x + size, 0 ) );

    return pressed;
}

void drawFolderIcon( ImDrawList* drawList,
                     ImVec2 offset,
                     float32 size,
                     float32 rounding,
                     uint32_t colorFolder,
                     uint32_t colorFolderLid ) {
    float32 verticalSize = size * 0.75f;
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    ImVec2 halfSize( size * 0.5f, verticalSize * 0.5f );
    float32 tabHeight = verticalSize * 0.2f; // Height of the top section
    float32 tabWidth = size * 0.3f;          // Width of the top section
    float32 openAngle = verticalSize * 0.1f; // Opening effect
    ImVec2 center = ImVec2( cursorPos.x + offset.x + halfSize.x, cursorPos.y + offset.y + halfSize.x );

    ImVec2 topLeft = center - halfSize;
    ImVec2 bottomRight = center + halfSize;
    // Draw folder body
    drawList->AddRectFilled( topLeft + ImVec2( 0, openAngle ),
                             bottomRight,
                             colorFolder,
                             rounding,
                             ImDrawFlags_RoundCornersBottom | ImDrawFlags_RoundCornersTopRight );

    // Draw the top section
    ImVec2 lidPoints[ 5 ] = { topLeft, // Top-left of the lid
                              topLeft + ImVec2( tabWidth, 0.0f ),
                              topLeft + ImVec2( size * 0.5f, openAngle ),
                              topLeft + ImVec2( tabWidth, tabHeight ),
                              topLeft + ImVec2( 0, tabHeight ) };

    drawList->PathClear();
    drawList->PathArcToFast( lidPoints[ 0 ] + ImVec2( rounding, rounding ), rounding, 6, 9 );
    drawList->PathLineTo( lidPoints[ 1 ] );

    drawList->PathLineTo( lidPoints[ 2 ] );
    drawList->PathLineTo( lidPoints[ 3 ] );
    drawList->PathLineTo( lidPoints[ 4 ] );
    drawList->PathLineTo( lidPoints[ 0 ] );
    drawList->PathFillConvex( colorFolderLid );
}

void drawFileIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color ) {
    const float foldSize = size * 0.25f;
    const float fullH = size * 1.25f;

    ImVec2 pos = ImGui::GetCursorScreenPos() + offset;
    const ImVec2 topLeft = pos;
    const ImVec2 bottomRight = pos + ImVec2( size, fullH );

    const ImVec2 foldStart( bottomRight.x - foldSize, topLeft.y );
    const ImVec2 foldTip( bottomRight.x, topLeft.y + foldSize );

    drawList->PathClear();
    drawList->PathLineTo( topLeft );                                // TL
    drawList->PathLineTo( foldStart );                              // cut start
    drawList->PathLineTo( foldTip );                                // cut end
    drawList->PathLineTo( ImVec2( bottomRight.x, bottomRight.y ) ); // BR
    if( rounding > 0.0f ) {
        drawList->PathArcToFast( ImVec2( topLeft.x + rounding, bottomRight.y - rounding ), rounding, 3, 6 );
        drawList->PathArcToFast( ImVec2( topLeft.x + rounding, topLeft.y + rounding ), rounding, 6, 9 );
    } else {
        drawList->PathLineTo( ImVec2( topLeft.x, bottomRight.y ) ); // BL
        drawList->PathLineTo( topLeft );                            // back to TL
    }
    drawList->PathStroke( color.toABGR() );

    // Fold triangle — the dog-ear flap
    drawList->AddTriangle( foldStart, foldTip, ImVec2( foldStart.x, foldTip.y ), color.toABGR() );

    // // Optional: a couple of faint lines to suggest text on the page
    // const uint32_t lineColor = ( color.toABGR() & 0x00FFFFFF ) | 0x28000000; // 16% alpha of body color
    // const float lineX0 = topLeft.x + size * 0.15f;
    // const float lineX1 = bottomRight.x - size * 0.15f;
    // const float lineStep = fullH * 0.15f;
    // for( int i = 0; i < 3; ++i ) {
    //     const float y = topLeft.y + foldSize + lineStep * ( i + 1 );
    //     drawList->AddLine( ImVec2( lineX0, y ), ImVec2( lineX1, y ), lineColor, 1.0f );
    // }
}

void drawPlusIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 center = cursorPos - offset;
    float thickness = size * 0.2f;
    float halfLen = size * 0.5f;
    float halfThick = thickness * 0.5f;

    float gap = halfThick - 0.5f; // length to trim from each horizontal bar end to avoid overdraw

    // --- Left horizontal rectangle (half bar)
    drawList->PathLineTo( ImVec2( center.x - halfLen, center.y - halfThick ) );
    drawList->PathLineTo( ImVec2( center.x - gap, center.y - halfThick ) );
    drawList->PathLineTo( ImVec2( center.x - gap, center.y + halfThick ) );
    drawList->PathLineTo( ImVec2( center.x - halfLen, center.y + halfThick ) );
    drawList->PathFillConvex( color );

    // --- Right horizontal rectangle (half bar)
    drawList->PathLineTo( ImVec2( center.x + gap, center.y - halfThick ) );
    drawList->PathLineTo( ImVec2( center.x + halfLen, center.y - halfThick ) );
    drawList->PathLineTo( ImVec2( center.x + halfLen, center.y + halfThick ) );
    drawList->PathLineTo( ImVec2( center.x + gap, center.y + halfThick ) );
    drawList->PathFillConvex( color );

    // --- Full vertical rectangle
    drawList->PathLineTo( ImVec2( center.x - halfThick, center.y - halfLen ) );
    drawList->PathLineTo( ImVec2( center.x + halfThick, center.y - halfLen ) );
    drawList->PathLineTo( ImVec2( center.x + halfThick, center.y + halfLen ) );
    drawList->PathLineTo( ImVec2( center.x - halfThick, center.y + halfLen ) );
    drawList->PathFillConvex( color );

    // ImGui::setcurs(ImGui::GetCursorPosX() - offset.x + size);
}

void drawMinusIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 center = cursorPos - offset;

    // Define the size of the "X" lines
    float32 lineThickness = size * 0.2f; // Thickness of the lines
    float32 lineLength = size * 1.0f;    // Length of the lines

    // Calculate the end points for the diagonal lines
    ImVec2 lineStart = ImVec2( center.x - lineLength * 0.5f, center.y );
    ImVec2 lineEnd = ImVec2( center.x + lineLength * 0.5f, center.y );

    // Draw the diagonal lines
    drawList->AddLine( lineStart, lineEnd, color, lineThickness );

    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x + size);
}

void drawXIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 center = cursorPos - offset;

    float thickness = size * 0.2f;
    float halfLen = size * 0.5f;
    float gap = thickness * 0.5f; // Avoid overlap by trimming to center

    // Diagonal directions
    constexpr float32 S2 = std::numbers::sqrt2_v< float32 > / 2.0f;
    ImVec2 dir1 = ImVec2( S2, S2 );  // direction top left to bottom right
    ImVec2 dir2 = ImVec2( -S2, S2 ); // direction bottom left to top right

    ImVec2 ortho1 = ImVec2( -dir1.y, dir1.x ) * ( thickness * 0.5f );
    ImVec2 ortho2 = ImVec2( -dir2.y, dir2.x ) * ( thickness * 0.5f );

    auto drawHalf = [ & ]( ImVec2 from, ImVec2 to, ImVec2 ortho ) {
        drawList->PathLineTo( from + ortho );
        drawList->PathLineTo( from - ortho );
        drawList->PathLineTo( to - ortho );
        drawList->PathLineTo( to + ortho );
        drawList->PathFillConvex( color );
    };

    // Diagonal 1:
    drawHalf( center - dir1 * halfLen, center - dir1 * gap, ortho1 );
    drawHalf( center + dir1 * gap, center + dir1 * halfLen, ortho1 );

    // Diagonal 2:
    drawHalf( center - dir2 * halfLen, center - dir2 * gap, ortho2 );
    drawHalf( center + dir2 * gap, center + dir2 * halfLen, ortho2 );

    // Center
    ImVec2 p1 = center - dir1 * gap + ortho1;
    ImVec2 p2 = center - dir2 * gap + ortho2;
    ImVec2 p3 = center + dir1 * gap - ortho1;
    ImVec2 p4 = center + dir2 * gap - ortho2;

    drawList->PathLineTo( p1 );
    drawList->PathLineTo( p2 );
    drawList->PathLineTo( p3 );
    drawList->PathLineTo( p4 );
    drawList->PathFillConvex( color );
}

void drawDivisionIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 center = cursorPos - offset;

    float32 lineThickness = size * 0.2f;
    float32 lineLength = size * 1.0f;
    float32 dotRadius = size * 0.1f;

    // Horizontal line
    ImVec2 lineStart = ImVec2( center.x - lineLength * 0.5f, center.y );
    ImVec2 lineEnd = ImVec2( center.x + lineLength * 0.5f, center.y );
    drawList->AddLine( lineStart, lineEnd, color, lineThickness );

    // Top dot
    ImVec2 topDotCenter = ImVec2( center.x, center.y - size * 0.4f );
    drawList->AddCircleFilled( topDotCenter, dotRadius, color );

    // Bottom dot
    ImVec2 bottomDotCenter = ImVec2( center.x, center.y + size * 0.4f );
    drawList->AddCircleFilled( bottomDotCenter, dotRadius, color );
}

// void drawFilterIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color ) {
//     // ImVec2 cursorPos = ImGui::GetCursorScreenPos();
//     // ImVec2 center = cursorPos - offset;
//     // uint32_t colorARGB = color.toABGR();
//
//
//
// }

void drawPinIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color ) {
    drawPinIcon( drawList, offset, size, rounding, color.toABGR() );
}

void drawPinIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    float32 halfSize = size * 0.5f;
    ImVec2 center = ImVec2( std::round( cursorPos.x + halfSize + offset.x ),
                            std::round( cursorPos.y + halfSize + offset.y ) );

    const float32 lineThickness = std::max( 1.0f, std::round( size * 0.08f ) );

    // Pin head — circle at top
    const float32 headRadius = size * 0.28f;
    const ImVec2 headCenter = ImVec2( center.x, center.y - size * 0.18f );

    drawList->AddCircleFilled( headCenter, headRadius, color );
    drawList->AddCircleFilled( headCenter, headRadius * 0.45f, ImGui::GetColorU32( ImGuiCol_WindowBg ) );

    // Pin needle — line from bottom of head down to a point
    const float32 needleTop = headCenter.y + headRadius * 0.7f;
    const float32 needleBottom = center.y + size * 0.48f;

    drawList->AddLine( ImVec2( center.x, needleTop ), ImVec2( center.x, needleBottom ), color, lineThickness );

    ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset.x + size );
}

void drawFilterIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 base = ImVec2( cursorPos.x + offset.x, cursorPos.y + offset.y );

    // All points relative to size, matching the funnel shape
    const float32 lineThickness = std::max( 1.0f, std::round( size * 0.05f ) );

    ImVec2 pts[ 6 ] = {
        { base.x + size * 0.00f, base.y + size * 0.00f }, // top-left
        { base.x + size * 1.00f, base.y + size * 0.00f }, // top-right
        { base.x + size * 0.62f, base.y + size * 0.50f }, // mid-right
        { base.x + size * 0.62f, base.y + size * 1.00f }, // bottom-right stem
        { base.x + size * 0.38f, base.y + size * 0.75f }, // bottom-left stem
        { base.x + size * 0.38f, base.y + size * 0.50f }, // mid-left
    };

    drawList->PathClear();
    for( const auto& p : pts )
        drawList->PathLineTo( p );
    drawList->PathStroke( color, ImDrawFlags_Closed, lineThickness );
}

void drawFilledFilterIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, uint32_t color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 base = ImVec2( cursorPos.x + offset.x, cursorPos.y + offset.y );

    ImVec2 pts[ 6 ] = {
        { base.x + size * 0.00f, base.y + size * 0.00f }, // top-left
        { base.x + size * 1.00f, base.y + size * 0.00f }, // top-right
        { base.x + size * 0.62f, base.y + size * 0.50f }, // mid-right
        { base.x + size * 0.62f, base.y + size * 1.00f }, // bottom-right stem
        { base.x + size * 0.38f, base.y + size * 0.75f }, // bottom-left stem
        { base.x + size * 0.38f, base.y + size * 0.50f }, // mid-left
    };

    drawList->PathClear();
    for( const auto& p : pts )
        drawList->PathLineTo( p );
    drawList->PathFillConcave( color );
}

void drawMovieCameraIcon( ImDrawList* drawList, ImVec2 offset, float32 size, Color color, Color accent ) {
    uint32_t colorARGB = color.toABGR();
    uint32_t accentARGB = accent.toABGR();

    ImVec2 currentPosition = ImGui::GetCursorScreenPos();
    float32 x = currentPosition.x + offset.x;
    float32 y = currentPosition.y + offset.y;

    const float32 w = size * 0.75f;
    const float32 finW = size * 0.25f;
    const float32 bodyH = size * 0.5625f;
    const float32 r = size * 0.044f;
    const float32 sr = size * 0.1375f;
    const float32 lr = size * 0.2f;

    // Local-space Y centers (relative to top of bounding box)
    const float32 lcyLocal = size * 0.1125f;
    const float32 scyLocal = size * 0.1825f;
    const float32 bodyYLocal = size * 0.3125f;

    // True vertical extents in local space
    const float32 minY = std::min( lcyLocal - lr, scyLocal - sr ); // topmost edge
    const float32 maxY = bodyYLocal + bodyH;                       // bottommost edge
    const float32 totalH = maxY - minY;

    // Shift so content is vertically centered within size
    const float32 yOffset = ( size - totalH ) * 0.5f - minY;

    const float32 scx = x + size * 0.265f;
    const float32 scy = y + scyLocal + yOffset;
    const float32 lcx = x + size * 0.5675f;
    const float32 lcy = y + lcyLocal + yOffset;
    const float32 bodyY = y + bodyYLocal + yOffset;

    // Small roll (left)
    drawList->AddCircleFilled( ImVec2( scx, scy ), sr, colorARGB );
    drawList->AddCircleFilled( ImVec2( scx, scy ), sr * 0.45f, accentARGB );

    // Large roll (right)
    drawList->AddCircleFilled( ImVec2( lcx, lcy ), lr, colorARGB );
    drawList->AddCircleFilled( ImVec2( lcx, lcy ), lr * 0.44f, accentARGB );

    // Body
    drawList->AddRectFilled( ImVec2( x, bodyY ), ImVec2( x + w, bodyY + bodyH ), colorARGB, r );

    // Viewport window
    const float32 vpPadX = size * 0.088f;
    const float32 vpPadY = size * 0.088f;
    drawList->AddRectFilled( ImVec2( x + vpPadX, bodyY + vpPadY ),
                             ImVec2( x + w * 0.825f, bodyY + bodyH - vpPadY ),
                             accentARGB,
                             r * 0.6f );

    // Lens fin
    const float32 finIns = bodyH * 0.155f;
    drawList->AddQuadFilled( ImVec2( x + w, bodyY + finIns ),
                             ImVec2( x + w + finW, bodyY ),
                             ImVec2( x + w + finW, bodyY + bodyH ),
                             ImVec2( x + w, bodyY + bodyH - finIns ),
                             colorARGB );
}

void drawSimpleMovieCameraIcon( ImDrawList* drawList,
                                ImVec2 offset,
                                float32 size,
                                Color bodyColor,
                                Color innerBodyColor,
                                Color lensColor ) {
    ImVec2 currentPosition = ImGui::GetCursorScreenPos();
    float x = currentPosition.x + offset.x;
    float y = currentPosition.y + offset.y;
    float w = size;
    float h = size * 0.667f;
    float r = ImMax( 1.0f, size * 0.04f );

    uint32_t bodyARGB = bodyColor.toABGR();
    uint32_t innerBodyARGB = innerBodyColor.toABGR();
    uint32_t lensARGB = lensColor.toABGR();

    // Body
    drawList->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), bodyARGB, r );

    // Viewport window
    float pad = size * 0.08f;
    drawList->AddRect( ImVec2( x + pad, y + pad ),
                       ImVec2( x + w * 0.825f, y + h - pad ),
                       innerBodyARGB,
                       ImMax( 1.0f, r * 0.6f ) );

    // Fin
    float finW = size * 0.29f;
    float finIns = h * 0.15f;
    drawList->AddQuad( ImVec2( x + w, y + finIns ),
                       ImVec2( x + w + finW, y ),
                       ImVec2( x + w + finW, y + h ),
                       ImVec2( x + w, y + h - finIns ),
                       lensARGB );
}

void drawGridIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color ) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    const float32 lineThickness = size * 0.08f;
    const float32 halfThick = lineThickness * 0.5f;
    const int32_t divisions = 3;
    const uint32_t colorARGB = color.toABGR();

    ImVec2 center = ImVec2( cursorPos.x + offset.x, cursorPos.y + offset.y );
    ImVec2 topLeft = ImVec2( center.x + halfThick, center.y + halfThick );
    ImVec2 bottomRight = ImVec2( center.x + size - halfThick, center.y + size - halfThick );

    // Outer rounded border
    drawList->AddRect( topLeft, bottomRight, colorARGB, rounding, ImDrawFlags_None, lineThickness );

    const float32 divisionsF = numericCast< float32 >( divisions );
    // Step evenly across the full size, so division lines land on consistent pixel boundaries
    // regardless of what the inner padding works out to
    const float32 step = std::round( size / divisionsF );

    const float32 innerLeft = topLeft.x + halfThick;
    const float32 innerRight = bottomRight.x - halfThick;
    const float32 innerTop = topLeft.y + halfThick;
    const float32 innerBottom = bottomRight.y - halfThick;

    for( int32_t i = 1; i < divisions; ++i ) {
        float32 x = center.x + step * numericCast< float32 >( i );
        float32 y = center.y + step * numericCast< float32 >( i );
        drawList->AddLine( ImVec2( x, innerTop ), ImVec2( x, innerBottom ), colorARGB, lineThickness );
        drawList->AddLine( ImVec2( innerLeft, y ), ImVec2( innerRight, y ), colorARGB, lineThickness );
    }

    ImGui::SetCursorPosX( ImGui::GetCursorPosX() + offset.x + size );
}

} // namespace onyx::ui
#endif
