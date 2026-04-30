
#include <onyx/ui/controls/colorcontrol.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/scopeddisable.h>

#if ONYX_IS_EDITOR

#include <onyx/assets/assetsystem.h>
#include <onyx/ui/controls/assetselector.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::ui::property_grid {
namespace {
Stack< ImGuiID > g_locPropertyGridIdStack;
String g_locPropertyGridTooltip;

constexpr uint32_t BackgroundChannel = 0;
constexpr uint32_t ForegroundChannel = 1;

float32 g_locSplitterMinX;

void drawSplitter() {
    ImGuiID propertyGridID = g_locPropertyGridIdStack.top();

    ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
    uint32_t splitterId = imguiStateStorage->GetInt( propertyGridID );
    float& storedSplitterPosX = *imguiStateStorage->GetFloatRef( splitterId, g_locSplitterMinX );

    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 size = ImGui::GetItemRectSize();
    const float lineWidth = style.DockingSeparatorSize;
    ImVec2 screenPos = ImGui::GetCursorScreenPos();

    ImRect visualBB;
    visualBB.Min = ImVec2( screenPos.x + storedSplitterPosX - lineWidth, screenPos.y - size.y - style.FramePadding.y );
    visualBB.Max = ImVec2( screenPos.x + storedSplitterPosX, screenPos.y - style.FramePadding.y );

    ImRect interactionBB = visualBB;
    interactionBB.Min.x -= 1.0f;
    interactionBB.Max.x += 1.0f;

    bool isHovered = false;
    bool isHeld = false;
    ImGuiID splitterButtonId = ImGui::GetID( "##splitter" );
    ImGui::ItemAdd( visualBB, splitterButtonId, nullptr, ImGuiItemFlags_NoNav );
    ImGui::ButtonBehavior( interactionBB, splitterButtonId, &isHovered, &isHeld, ImGuiButtonFlags_FlattenChildren );

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSetCurrent( BackgroundChannel );

    const uint32_t splitterColor = isHeld ? ImGui::GetColorU32( ImGuiCol_SeparatorActive )
                                          : ( isHovered ? ImGui::GetColorU32( ImGuiCol_SeparatorHovered )
                                                        : ImGui::GetColorU32( ImGuiCol_Separator ) );
    drawList->AddRectFilled( visualBB.Min, visualBB.Max, splitterColor );

    drawList->ChannelsSetCurrent( ForegroundChannel );
    // Handle dragging the splitter
    if( isHeld ) {
        // Update splitter position as the user drags it
        storedSplitterPosX = std::max( storedSplitterPosX + ImGui::GetIO().MouseDelta.x, g_locSplitterMinX );
    }

    // Change cursor on hover to indicate it's resizable
    if( isHovered ) {
        ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );
    }
}
} // namespace

void beginPropertyGrid( StringView propertyGrid, float32 splitMinX ) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit( 2 );
    drawList->ChannelsSetCurrent( ForegroundChannel );

    ImGui::BeginGroup();

    g_locPropertyGridIdStack.push( ImGui::GetID( propertyGrid.data() ) );
    ImGuiID id = g_locPropertyGridIdStack.top();

    ImGui::PushID( id );
    ImGui::BeginGroup();

    String splitterIdStr( propertyGrid );
    splitterIdStr += "_splitterX";
    ImGuiID splitterPositionXId = ImGui::GetID( splitterIdStr.data() );

    ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
    imguiStateStorage->SetInt( id, static_cast< int32_t >( splitterPositionXId ) );
    g_locSplitterMinX = splitMinX;
}

void endPropertyGrid() {
    ImGui::EndGroup();
    ImGui::PopID();

    drawSplitter();

    ImGui::EndGroup();

    g_locPropertyGridIdStack.pop();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsMerge();
}

void drawPropertyName( StringView propertyName ) {
    ImGuiID propertyGridID = g_locPropertyGridIdStack.top();

    ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
    uint32_t splitterId = imguiStateStorage->GetInt( propertyGridID );
    float32 splitterPosX = imguiStateStorage->GetFloat( splitterId );

    const ImGuiStyle& style = ImGui::GetStyle();
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();

    ImGui::BeginHorizontal( propertyName.data() );

    const char* label = propertyName.data();
    const char* label_end = ImGui::FindRenderedTextEnd( label );
    ImVec2 label_size = ImGui::CalcTextSize( label, label_end, true );
    ImVec2 label_pos = window->DC.CursorPos;
    float32 indendation = window->DC.Indent.x;

    // Determine the height of the cell
    float cellHeight = ImGui::GetTextLineHeightWithSpacing();

    // Calculate vertical alignment offset to center the text
    float verticalOffset = ( cellHeight - label_size.y ) * 0.5f;

    // Adjust position for centered text
    label_pos.y += verticalOffset;

    float32 ellipsis_max = label_pos.x + splitterPosX - style.ItemSpacing.x - indendation;
    ImVec2 label_pos_max = ImVec2( ellipsis_max, label_pos.y + label_size.y );
    ImGui::PushFont( ImGui::GetDefaultFont() );

    // Render the text with ellipsis if it exceeds the available width
    ImGui::RenderTextEllipsis( ImGui::GetWindowDrawList(),
                               label_pos,
                               label_pos_max,
                               ellipsis_max,
                               ellipsis_max,
                               label,
                               label_end,
                               &label_size );

    ImGui::PopFont();

    // move into name group
    const bool hasTooltip = g_locPropertyGridTooltip.empty() == false;
    if( hasTooltip ) {
        auto cursorPos = ImGui::GetCursorPos();
        ImGui::PushClipRect( label_pos, label_pos_max, true );
        DrawInfoIcon( ImGui::GetWindowDrawList(),
                      ImVec2( label_size.x + style.ItemSpacing.x, verticalOffset ),
                      ImGui::GetTextLineHeight() / 2.0f,
                      0x33FFFFFF );
        ImGui::PopClipRect();
        ImGui::SetCursorPos( cursorPos );
    }

    ImGui::Dummy( ImVec2( splitterPosX - indendation + style.DockingSeparatorSize + 2 * style.ItemInnerSpacing.x,
                          ImGui::GetFrameHeightWithSpacing() ) );

    if( hasTooltip && ImGui::BeginItemTooltip() ) {
        ImGui::TextEx( g_locPropertyGridTooltip.c_str() );
        ImGui::EndTooltip();
    }

    g_locPropertyGridTooltip.clear();
}

void drawPropertyValue( const InplaceFunction< void(), 64 >& functor ) {
    functor();
    ImGui::EndHorizontal();
}

bool beginPropertyGroup( StringView propertyName ) {
    ImGui::PushID( propertyName.data() );

    drawPropertyName( propertyName );
    ImGui::EndHorizontal();

    ImGui::Indent();
    ImGui::BeginGroup();

    return true;
}

bool beginPropertyGroup( StringView propertyName, const InplaceFunction< bool() >& customHeader ) {
    ImGui::PushID( propertyName.data() );

    drawPropertyName( propertyName );
    bool showGroup = customHeader();
    ImGui::EndHorizontal();

    if( showGroup ) {
        ImGui::Indent();
        ImGui::BeginGroup();
    } else {
        ImGui::PopID();
    }

    return showGroup;
}

bool beginCollapsiblePropertyGroup( StringView propertyName, ImGuiTreeNodeFlags flags ) {
    ImGui::PushID( propertyName.data() );

    ScopedImGuiStyle style{
        { ImGuiStyleVar_FrameBorderSize, 0.0f },
    };

    if( ContextMenuHeader( propertyName.data(), flags | ImGuiTreeNodeFlags_Framed ) ) {
        ImGui::Indent();
        ImGui::BeginGroup();
        return true;
    } else {
        ImGui::PopID();
        return false;
    }
}

bool beginCollapsiblePropertyGroup( StringView propertyName,
                                    const InplaceFunction< bool() >& customHeader,
                                    ImGuiTreeNodeFlags flags ) {
    ImGui::PushID( propertyName.data() );

    ScopedImGuiStyle style{
        { ImGuiStyleVar_FrameBorderSize, 0.0f },
    };

    if( ContextMenuHeader( propertyName.data(), customHeader, flags | ImGuiTreeNodeFlags_Framed ) ) {
        ImGui::Indent();
        ImGui::BeginGroup();
        return true;
    } else {
        ImGui::PopID();
        return false;
    }
}

void endPropertyGroup() {
    ImGui::EndGroup();
    ImGui::Unindent();

    ImGui::PopID();
}

void setNextPropertyTooltip( const String& tooltip ) {
    g_locPropertyGridTooltip = tooltip;
}

bool drawButton( StringView propertyName ) {
    drawPropertyName( format::format( "##{}", propertyName ) );

    bool isPressed = ImGui::Button( propertyName.data() );
    ImGui::EndHorizontal();

    return isPressed;
}

bool drawProperty( StringView propertyName, StringView readOnlyValue ) {
    drawPropertyName( propertyName );

    bool hasModified = false;
    {
        ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
        ScopedImGuiDisabled disabled;
        hasModified = DrawStringInput( format::format( "##{}", propertyName ),
                                       readOnlyValue,
                                       ImVec2( 0, 0 ),
                                       ImGuiInputTextFlags_ReadOnly );
    }

    ImGui::EndHorizontal();

    return hasModified;
}

bool drawProperty( StringView propertyName, String& value ) {
    return drawProperty( propertyName, value, ImGuiInputTextFlags_None );
}

bool drawProperty( StringView propertyName, String& value, ImGuiInputTextFlags textFlags ) {
    drawPropertyName( propertyName );

    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    bool hasModified = DrawStringInput( format::format( "##{}", propertyName ), value, ImVec2( 0, 0 ), textFlags );

    ImGui::EndHorizontal();

    return hasModified;
}

bool drawAssetSelector( StringView propertyName, assets::AssetId& outAssetId, assets::AssetType assetType ) {
    drawPropertyName( propertyName );

    ImGui::PushID( propertyName.data() );

    bool hasModified = AssetSelector( *g_uiContext.AssetSystem, assetType, outAssetId );

    ImGui::PopID();
    ImGui::EndHorizontal();

    return hasModified;
}

bool drawProperty( StringView propertyName, bool& value ) {
    drawPropertyName( propertyName );

    // Draw Value
    bool hasModified = false;
    {
        ScopedImGuiId valueId( propertyName );
        ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
        hasModified = ImGui::Checkbox( "##inputBool", &value );
    }

    ImGui::EndHorizontal();

    return hasModified;
}

bool drawColorProperty( StringView propertyName, Vector3f32& inOutColor ) {
    drawPropertyName( propertyName );
    ImGui::EndHorizontal();

    // Draw Value outside horizontal group as its not working with the layout
    ImGui::SameLine();
    bool hasModified = false;
    // float color[3] = { inOutColor[0], inOutColor[1], inOutColor[2] };
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if( ui::ColorInput( "##inputColor", inOutColor ) ) {
        hasModified = true;
        // inOutColor[0] = color[0];
        // inOutColor[1] = color[1];
        // inOutColor[2] = color[2];
    }

    return hasModified;
}

bool drawColorProperty( StringView propertyName, Vector4f32& inOutColor ) {
    drawPropertyName( propertyName );
    ImGui::EndHorizontal();

    // Draw Value outside horizontal group as its not working with the layout
    ImGui::SameLine();
    bool hasModified = false;
    float color[ 4 ] = { inOutColor[ 0 ], inOutColor[ 1 ], inOutColor[ 2 ], inOutColor[ 3 ] };
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    if( ImGui::ColorEdit4( "##inputColor",
                           color,
                           ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf ) ) {
        hasModified = true;
        inOutColor[ 0 ] = color[ 0 ];
        inOutColor[ 1 ] = color[ 1 ];
        inOutColor[ 2 ] = color[ 2 ];
        inOutColor[ 3 ] = color[ 3 ];
    }

    return hasModified;
}

bool drawColorProperty( StringView propertyName, Vector4u8& inOutColor ) {
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    Vector4f32 color{ numericCast< float32 >( inOutColor[ 0 ] ) / 255.0f,
                      numericCast< float32 >( inOutColor[ 1 ] ) / 255.0f,
                      numericCast< float32 >( inOutColor[ 2 ] ) / 255.0f,
                      numericCast< float32 >( inOutColor[ 3 ] ) / 255.0f };
    if( drawColorProperty( propertyName, color ) ) {
        inOutColor[ 0 ] = numericCast< uint8_t >( color[ 0 ] * 255.0f );
        inOutColor[ 1 ] = numericCast< uint8_t >( color[ 1 ] * 255.0f );
        inOutColor[ 2 ] = numericCast< uint8_t >( color[ 2 ] * 255.0f );
        inOutColor[ 3 ] = numericCast< uint8_t >( color[ 3 ] * 255.0f );
        return true;
    }

    return false;
}
} // namespace onyx::ui::property_grid
#endif
