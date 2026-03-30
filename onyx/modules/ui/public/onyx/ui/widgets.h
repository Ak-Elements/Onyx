#pragma once

#if ONYX_USE_IMGUI

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::localization {
struct LocalizedString;
}

struct ImDrawList;
struct ImVec2;

namespace onyx::ui {
// TODO maybe move to a common gui file?
template < typename T >
static constexpr ImGuiDataType GetImGuiDataType() {
    if constexpr( std::is_same_v< T, int8_t > )
        return ImGuiDataType_S8;
    else if constexpr( std::is_same_v< T, int16_t > )
        return ImGuiDataType_S16;
    else if constexpr( std::is_same_v< T, int32_t > )
        return ImGuiDataType_S32;
    else if constexpr( std::is_same_v< T, int64_t > )
        return ImGuiDataType_S64;
    else if constexpr( std::is_same_v< T, uint8_t > )
        return ImGuiDataType_U8;
    else if constexpr( std::is_same_v< T, uint16_t > )
        return ImGuiDataType_U16;
    else if constexpr( std::is_same_v< T, uint32_t > )
        return ImGuiDataType_U32;
    else if constexpr( std::is_same_v< T, uint64_t > )
        return ImGuiDataType_U64;
    else if constexpr( std::is_same_v< T, float32 > )
        return ImGuiDataType_Float;
    else if constexpr( std::is_same_v< T, float64 > )
        return ImGuiDataType_Double;

    return ImGuiDataType_COUNT;
}

void DrawItemBackground( float32 rounding, float32 borderThickness, uint32_t color );
void DrawItemBorder( float32 thickness, float32 rounding, uint32_t color );

bool DrawSearchBar( String& searchString, StringView hintLabel, bool& grabFocus );

bool ContextMenuHeader( const localization::LocalizedString& label, ImGuiTreeNodeFlags flags = 0 );
bool ContextMenuHeader( StringView label, ImGuiTreeNodeFlags flags = 0 );
bool ContextMenuHeader( StringView label, const InplaceFunction< bool() >& customHeader, ImGuiTreeNodeFlags flags = 0 );

bool DrawStringInput( StringView id, StringView hint, String& value );
bool DrawStringInput( StringView id, String& value, const ImVec2& size, ImGuiInputTextFlags flags );
bool DrawStringInput( StringView id, StringView hint, String& value, const ImVec2& size, ImGuiInputTextFlags flags );
bool DrawStringInput( StringView id, StringView value, const ImVec2& size, ImGuiInputTextFlags flags );
void DrawMultilineText( StringView text, ImVec2 bounds, bool showEllipsis );

bool DrawRenameInput( StringView id, String& outName, const ImVec2& size, bool& isSelected );

void DrawPlusIcon( ImDrawList* draw_list, ImVec2 pos, float32 size, uint32_t color );
void DrawMinusIcon( ImDrawList* draw_list, ImVec2 pos, float32 size, uint32_t color );
void DrawXIcon( ImDrawList* draw_list, ImVec2 pos, float32 size, uint32_t color );
void DrawDivisionIcon( ImDrawList* draw_list, ImVec2 pos, float32 size, uint32_t color );
void DrawInfoIcon( ImDrawList* draw_list, ImVec2 pos, float32 radius, uint32_t color );
void DrawSearchIcon( ImDrawList* draw_list, ImVec2 offset, float32 radius, uint32_t color );
bool DrawCloseButton( ImDrawList* draw_list, ImVec2 pos, float32 size, uint32_t color );
void DrawFolderIcon( ImDrawList* draw_list,
                     ImVec2 offset,
                     float32 size,
                     float32 rounding,
                     uint32_t color_folder,
                     uint32_t color_folder_lid );

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool DrawScalarInput( StringView label,
                      ImGuiDataType data_type,
                      ScalarT& data,
                      const void* p_step = nullptr,
                      const void* p_step_fast = nullptr,
                      const char* format = nullptr,
                      ImGuiInputTextFlags flags = ImGuiInputTextFlags_None ) {
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return false;

    ::ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    IM_ASSERT( ( flags & ImGuiInputTextFlags_EnterReturnsTrue ) ==
               0 ); // Not supported by InputScalar(). Please open an issue if you this would be useful to you.
                    // Otherwise use IsItemDeactivatedAfterEdit()!

    if( format == NULL )
        format = "{}";

    void* p_data_default = ( g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasRefVal ) ? &g.NextItemData.RefVal
                                                                                          : &g.DataTypeZeroValue;
    ScalarT data_default = *static_cast< ScalarT* >( p_data_default );

    InplaceString< 64 > buf;
    if( ( flags & ImGuiInputTextFlags_DisplayEmptyRefVal ) &&
        ImGui::DataTypeCompare( data_type, &data, p_data_default ) == 0 )
        buf[ 0 ] = 0;
    else
        format::formatTo2( buf, format, data );

    // Disable the MarkItemEdited() call in InputText but keep ImGuiItemStatusFlags_Edited.
    // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.
    g.NextItemData.ItemFlags |= ImGuiItemFlags_NoMarkEdited;
    flags |= ImGuiInputTextFlags_AutoSelectAll | (ImGuiInputTextFlags)ImGuiInputTextFlags_LocalizeDecimalPoint;

    bool value_changed = false;
    ScalarT newValue = data_default;
    if( p_step == NULL ) {
        if( ImGui::InputText( label.data(), buf.getData(), 64, flags ) ) {
            if( buf.empty() == false ) {
                std::from_chars( buf.getData(), buf.getData() + buf.getLength(), newValue );
            }

            if( newValue != data ) {
                data = newValue;
                value_changed = true;
            }
        }
    } else {
        const float button_size = ImGui::GetFrameHeight();

        ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g.
                             // IsItemActive()
        ImGui::PushID( label.data() );
        ImGui::SetNextItemWidth(
            ImMax( 1.0f, ImGui::CalcItemWidth() - ( button_size + style.ItemInnerSpacing.x ) * 2 ) );
        if( ImGui::InputText( "",
                              buf.getData(),
                              buf.getLength(),
                              flags ) ) // PushId(label) + "" gives us the expected ID from outside point of view
        {
            if( buf.empty() == false ) {
                std::from_chars( buf.getData(), buf.getData() + buf.getLength(), newValue );
            }

            if( newValue != data ) {
                data = newValue;
                value_changed = true;
            }
        }
        IMGUI_TEST_ENGINE_ITEM_INFO( g.LastItemData.ID,
                                     label,
                                     g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable );

        // Step buttons
        const ImVec2 backup_frame_padding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        if( flags & ImGuiInputTextFlags_ReadOnly )
            ImGui::BeginDisabled();
        ImGui::PushItemFlag( ImGuiItemFlags_ButtonRepeat, true );
        ImGui::SameLine( 0, style.ItemInnerSpacing.x );
        if( ImGui::ButtonEx( "-", ImVec2( button_size, button_size ) ) ) {
            ImGui::DataTypeApplyOp( data_type, '-', &data, &data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step );
            value_changed = true;
        }
        ImGui::SameLine( 0, style.ItemInnerSpacing.x );
        if( ImGui::ButtonEx( "+", ImVec2( button_size, button_size ) ) ) {
            ImGui::DataTypeApplyOp( data_type, '+', &data, &data, g.IO.KeyCtrl && p_step_fast ? p_step_fast : p_step );
            value_changed = true;
        }
        ImGui::PopItemFlag();
        if( flags & ImGuiInputTextFlags_ReadOnly )
            ImGui::EndDisabled();

        ////TODO: Maybe for the future Drag behavior
        // ImGuiID id = ImGui::GetID(label.data());
        // value_changed = ImGui::DragBehavior(id, data_type, &data, 1.0f, nullptr, nullptr, format,
        // ImGuiSliderFlags_None); if (value_changed)
        //     ImGui::MarkItemEdited(id);

        const char* label_end = ImGui::FindRenderedTextEnd( label.data() );
        if( label != label_end ) {
            ImGui::SameLine( 0, style.ItemInnerSpacing.x );
            ImGui::TextEx( label.data(), label_end );
        }
        style.FramePadding = backup_frame_padding;

        ImGui::PopID();
        ImGui::EndGroup();
    }

    g.LastItemData.ItemFlags &= ~ImGuiItemFlags_NoMarkEdited;
    if( value_changed )
        ImGui::MarkItemEdited( g.LastItemData.ID );

    return value_changed;
}
} // namespace onyx::ui
#endif