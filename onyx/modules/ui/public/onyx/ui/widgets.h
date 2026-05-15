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
static constexpr ImGuiDataType getImGuiDataType() {
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

void drawCustomWindowTitleBar( StringView title, const InplaceFunction< void() >& customFunctor );
void drawPinnableWindowTitleBar( StringView title, bool& isPinned );

void drawItemBackground( float32 rounding, float32 borderThickness, uint32_t color );
void drawItemBorder( float32 thickness, float32 rounding, uint32_t color );

bool drawSearchBar( String& searchString, StringView hintLabel, bool& grabFocus );

bool contextMenuHeader( const localization::LocalizedString& label, ImGuiTreeNodeFlags flags = 0 );
bool contextMenuHeader( StringView label, ImGuiTreeNodeFlags flags = 0 );
bool contextMenuHeader( StringView label, const InplaceFunction< bool() >& customHeader, ImGuiTreeNodeFlags flags = 0 );

bool drawStringInput( StringView id, StringView hint, String& value );
bool drawStringInput( StringView id, String& value, const ImVec2& size, ImGuiInputTextFlags flags );
bool drawStringInput( StringView id, StringView hint, String& value, const ImVec2& size, ImGuiInputTextFlags flags );
bool drawStringInput( StringView id, StringView value, const ImVec2& size, ImGuiInputTextFlags flags );
void drawMultilineText( StringView text, ImVec2 bounds, bool showEllipsis );

bool drawRenameInput( StringView id, String& outName, const ImVec2& size, bool& isSelected );

void drawPlusIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color );
void drawMinusIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color );
void drawXIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color );
void drawDivisionIcon( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color );
void drawInfoIcon( ImDrawList* drawList, ImVec2 offset, float32 radius, uint32_t color );
void drawSearchIcon( ImDrawList* drawList, ImVec2 offset, float32 radius, uint32_t color );
bool drawCloseButton( ImDrawList* drawList, ImVec2 offset, float32 size, uint32_t color );
void drawFolderIcon( ImDrawList* drawList,
                     ImVec2 offset,
                     float32 size,
                     float32 rounding,
                     uint32_t colorFolder,
                     uint32_t colorFolderLid );

void drawPinIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color );
void drawPinIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, uint32_t color );

void drawMovieCameraIcon( ImDrawList* drawList, ImVec2 offset, float32 size, Color color, Color accent );
void drawSimpleMovieCameraIcon( ImDrawList* drawList,
                                ImVec2 offset,
                                float32 size,
                                Color bodyColor,
                                Color innerBodyColor,
                                Color lensColor );
void drawGridIcon( ImDrawList* drawList, ImVec2 offset, float32 size, float32 rounding, Color color );

template < typename ScalarT > requires std::is_arithmetic_v< ScalarT >
bool drawScalarInput( StringView label,
                      ImGuiDataType dataType,
                      ScalarT& data,
                      const void* pStep = nullptr,
                      const void* pStepFast = nullptr,
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

    if( format == nullptr )
        format = "{}";

    void* pDataDefault = ( g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasRefVal ) ? &g.NextItemData.RefVal
                                                                                        : &g.DataTypeZeroValue;
    ScalarT dataDefault = *static_cast< ScalarT* >( pDataDefault );

    InplaceString< 64 > buf;
    if( ( flags & ImGuiInputTextFlags_DisplayEmptyRefVal ) &&
        ImGui::DataTypeCompare( dataType, &data, pDataDefault ) == 0 )
        buf[ 0 ] = 0;
    else
        format::formatTo2( buf, format, data );

    // Disable the MarkItemEdited() call in InputText but keep ImGuiItemStatusFlags_Edited.
    // We call MarkItemEdited() ourselves by comparing the actual data rather than the string.
    g.NextItemData.ItemFlags |= ImGuiItemFlags_NoMarkEdited;
    flags |= ImGuiInputTextFlags_AutoSelectAll | (ImGuiInputTextFlags)ImGuiInputTextFlags_LocalizeDecimalPoint |
             ImGuiInputTextFlags_CharsDecimal;

    bool valueChanged = false;
    ScalarT newValue = dataDefault;
    if( pStep == nullptr ) {
        if( ImGui::InputText( label.data(), buf.getData(), 64, flags ) ) {
            if( buf.empty() == false ) {
                std::from_chars( buf.getData(), buf.getData() + buf.getLength(), newValue );
            }

            if( newValue != data ) {
                data = newValue;
                valueChanged = true;
            }
        }
    } else {
        const float buttonSize = ImGui::GetFrameHeight();

        ImGui::BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g.
                             // IsItemActive()
        ImGui::PushID( label.data() );
        ImGui::SetNextItemWidth(
            ImMax( 1.0f, ImGui::CalcItemWidth() - ( buttonSize + style.ItemInnerSpacing.x ) * 2 ) );
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
                valueChanged = true;
            }
        }
        IMGUI_TEST_ENGINE_ITEM_INFO( g.LastItemData.ID,
                                     label,
                                     g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable );

        // Step buttons
        const ImVec2 backupFramePadding = style.FramePadding;
        style.FramePadding.x = style.FramePadding.y;
        if( flags & ImGuiInputTextFlags_ReadOnly )
            ImGui::BeginDisabled();
        ImGui::PushItemFlag( ImGuiItemFlags_ButtonRepeat, true );
        ImGui::SameLine( 0, style.ItemInnerSpacing.x );
        if( ImGui::ButtonEx( "-", ImVec2( buttonSize, buttonSize ) ) ) {
            ImGui::DataTypeApplyOp( dataType, '-', &data, &data, g.IO.KeyCtrl && pStepFast ? pStepFast : pStep );
            valueChanged = true;
        }
        ImGui::SameLine( 0, style.ItemInnerSpacing.x );
        if( ImGui::ButtonEx( "+", ImVec2( buttonSize, buttonSize ) ) ) {
            ImGui::DataTypeApplyOp( dataType, '+', &data, &data, g.IO.KeyCtrl && pStepFast ? pStepFast : pStep );
            valueChanged = true;
        }
        ImGui::PopItemFlag();
        if( flags & ImGuiInputTextFlags_ReadOnly )
            ImGui::EndDisabled();

        ////TODO: Maybe for the future Drag behavior
        // ImGuiID id = ImGui::GetID(label.data());
        // value_changed = ImGui::DragBehavior(id, data_type, &data, 1.0f, nullptr, nullptr, format,
        // ImGuiSliderFlags_None); if (value_changed)
        //     ImGui::MarkItemEdited(id);

        const char* labelEnd = ImGui::FindRenderedTextEnd( label.data() );
        if( label != labelEnd ) {
            ImGui::SameLine( 0, style.ItemInnerSpacing.x );
            ImGui::TextEx( label.data(), labelEnd );
        }
        style.FramePadding = backupFramePadding;

        ImGui::PopID();
        ImGui::EndGroup();
    }

    g.LastItemData.ItemFlags &= ~ImGuiItemFlags_NoMarkEdited;
    if( valueChanged )
        ImGui::MarkItemEdited( g.LastItemData.ID );

    return valueChanged;
}
} // namespace onyx::ui
#endif