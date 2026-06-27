#pragma once

#if ONYX_IS_EDITOR

#include <onyx/ui/controls/button.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/theme/theme.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace onyx::ui {
class VectorControl {
  public:
    template < typename ScalarT >
    static bool vectorInput( Vector2< ScalarT >& vector ) {
        ImGui::BeginHorizontal( "##vec2Inputs" );

        constexpr ImGuiDataType DataType = getImGuiDataType< ScalarT >();

        ImGuiSystem* imguiSystem = static_cast< ImGuiSystem* >( ImGui::GetIO().UserData );
        const Theme& theme = imguiSystem->getTheme();

        bool hasModified = vectorComponentInput( "X", vector[ 0 ], DataType, 50.0f, theme.Colors.AxisX.toABGR() );
        hasModified |= vectorComponentInput( "Y", vector[ 1 ], DataType, 50.0f, theme.Colors.AxisY.toABGR() );

        ImGui::EndHorizontal();

        return hasModified;
    }

    template < typename ScalarT >
    static bool vectorInput( Vector3< ScalarT >& outVector ) {
        constexpr Vector3< ScalarT > MinValue{ std::numeric_limits< ScalarT >::lowest() };
        return vectorInput( outVector, MinValue );
    }

    template < typename ScalarT >
    static bool vectorInput( Vector3< ScalarT >& outVector, const Vector3< ScalarT >& minValue ) {
        constexpr InplaceArray< StringView, 3 > Labels{ "X", "Y", "Z" };
        return vectorInput( outVector, minValue, Labels );
    }

    template < typename ScalarT >
    static bool vectorInput( Vector3< ScalarT >& outVector, const InplaceArray< StringView, 3 >& labels ) {
        constexpr Vector3< ScalarT > MinValue{ std::numeric_limits< ScalarT >::lowest() };
        return vectorInput( outVector, MinValue, labels );
    }

    template < typename ScalarT >
    static bool vectorInput( Vector3< ScalarT >& outVector,
                             const Vector3< ScalarT >& minValue,
                             const InplaceArray< StringView, 3 >& labels ) {
        // float32 framePaddingX = ImGui::GetStyle().FramePadding.x;

        float32 componentInputSize = 50.0f;

        ImGuiSystem* imguiSystem = static_cast< ImGuiSystem* >( ImGui::GetIO().UserData );
        const Theme& theme = imguiSystem->getTheme();

        ScopedImGuiStyle style( ImGuiStyleVar_ItemSpacing, ImVec2( 10.0f, 0.0f ) );
        ImGui::BeginHorizontal( "##vec3Inputs" );

        constexpr ImGuiDataType DataType = getImGuiDataType< ScalarT >();

        // TODO: show error tooltip or error effect on UI when clamping (red flash)
        bool hasModified = vectorComponentInput( labels[ 0 ],
                                                 outVector.X,
                                                 DataType,
                                                 componentInputSize,
                                                 theme.Colors.AxisX.toABGR() );
        outVector.X = std::max( outVector.X, minValue.X );

        hasModified |= vectorComponentInput( labels[ 1 ],
                                             outVector.Y,
                                             DataType,
                                             componentInputSize,
                                             theme.Colors.AxisY.toABGR() );
        outVector.Y = std::max( outVector.Y, minValue.Y );

        hasModified |= vectorComponentInput( labels[ 2 ],
                                             outVector.Z,
                                             DataType,
                                             componentInputSize,
                                             theme.Colors.AxisZ.toABGR() );
        outVector.Z = std::max( outVector.Z, minValue.Z );

        ImGui::EndHorizontal();

        // RenderContextMenu(outVector);

        return hasModified;
    }

    template < typename ScalarT >
    static bool vectorInput( Vector4< ScalarT >& outVector ) {
        constexpr InplaceArray< StringView, 4 > Labels{ "X", "Y", "Z", "W" };
        return vectorInput( outVector, Labels );
    }

    template < typename ScalarT >
    static bool vectorInput( Vector4< ScalarT >& outVector, const InplaceArray< StringView, 4 >& labels ) {
        ImGuiSystem* imguiSystem = static_cast< ImGuiSystem* >( ImGui::GetIO().UserData );
        const Theme& theme = imguiSystem->getTheme();

        ImGui::BeginHorizontal( "##vec4Inputs" );

        constexpr ImGuiDataType DataType = getImGuiDataType< ScalarT >();

        bool hasModified = vectorComponentInput( labels[ 0 ],
                                                 outVector[ 0 ],
                                                 DataType,
                                                 50.0f,
                                                 theme.Colors.AxisX.toABGR() );
        hasModified |= vectorComponentInput( labels[ 1 ],
                                             outVector[ 1 ],
                                             DataType,
                                             50.0f,
                                             theme.Colors.AxisY.toABGR() );
        hasModified |= vectorComponentInput( labels[ 2 ],
                                             outVector[ 2 ],
                                             DataType,
                                             50.0f,
                                             theme.Colors.AxisZ.toABGR() );
        hasModified |= vectorComponentInput( labels[ 3 ],
                                             outVector[ 3 ],
                                             DataType,
                                             50.0f,
                                             theme.Colors.AxisW.toABGR() );

        ImGui::EndHorizontal();

        return hasModified;
    }

    template < typename ScalarT >
    static bool vectorComponentInput( StringView label,
                                      ScalarT& value,
                                      ImGuiDataType dataType,
                                      float32 valueColumnMinWidth,
                                      uint32_t backgroundColorARGB ) {
        // constexpr uint32_t INPUT_HOVERED_COLOR = 0xFFCCB399;
        // constexpr uint32_t INPUT_ACTIVE_COLOR = 0xFFCC884D;
        ScopedImGuiId scopedId( label );
        constexpr StringView InputIdString = "##hiddenInput";
        const ImGuiID inputId = ImGui::GetID( InputIdString.data() );

        bool isHovered = inputId == ImGui::GetHoveredID();
        bool isFocused = inputId == ImGui::GetActiveID();
        bool modified = false;

        {
            const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            const ImVec2 labelSize = { lineHeight + 3.0f, lineHeight };

            ScopedImGuiStyle styleOverride{
                { ImGuiStyleVar_FrameRounding, 3.0f },
                { ImGuiStyleVar_FrameBorderSize, 0.0f },
            };

            auto drawColoredLabel =
                [ & ]( const char* text, uint32_t backgroundColor, const ImVec2& size, ImFont* font ) {
                    ImGui::PushFont( font );
                    // Calculate the size of the text
                    ImVec2 textSize = ImGui::CalcTextSize( text );

                    // Calculate the position to center the text in the rectangle
                    // ImVec2 rectStartPos = ImGui::GetCursorPos();
                    ImVec2 rectScreenStartPos = ImGui::GetCursorScreenPos();
                    ImVec2 rectScreenEndPos = ImVec2( rectScreenStartPos.x + size.x, rectScreenStartPos.y + size.y );

                    ImGui::GetWindowDrawList()->AddRectFilled( rectScreenStartPos,
                                                               rectScreenEndPos,
                                                               backgroundColor,
                                                               3,
                                                               ImDrawFlags_RoundCornersLeft );

                    ImVec2 textStartPos = ImVec2( rectScreenStartPos.x + ( size.x - textSize.x ) * 0.5f,
                                                  rectScreenStartPos.y + ( size.y - textSize.y ) * 0.5f );

                    ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::SetCursorScreenPos( textStartPos );

                    ImGui::TextUnformatted( text );

                    // Restore the cursor position to the end of the rectangle
                    ImGui::SetCursorScreenPos( ImVec2( rectScreenStartPos.x + size.x, rectScreenStartPos.y ) );
                    ImGui::PopFont();
                };

            ImGui::BeginGroup();
            drawColoredLabel( label.data(), backgroundColorARGB, labelSize, GImGui->Font );
            modified = customInput( inputId,
                                    InputIdString,
                                    value,
                                    dataType,
                                    ImVec2( valueColumnMinWidth, lineHeight ) );
            ImGui::EndGroup();
        }

        ImGuiStyle& style = ImGui::GetStyle();
        if( style.FrameBorderSize > 0.0f ) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            uint32_t borderColor = isHovered   ? ImGui::GetColorU32( ImGuiCol_ButtonHovered )
                                   : isFocused ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                               : 0x00;
            drawList->AddRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), borderColor, style.FrameRounding );
        }

        return modified;
    }

  private:
    template < typename T >
    static bool renderContextMenu( T& vector ) {
        bool hasModified = false;
        if( ImGui::BeginPopupContextItem( "##ContextMenu", ImGuiPopupFlags_MouseButtonRight ) ) {
            if( ui::button( "Reset" ) ) {
                vector = T::Zero();
                ImGui::CloseCurrentPopup();
                hasModified = true;
            }

            ImGui::EndPopup();
        }

        return hasModified;
    }

    template < typename ScalarT >
    static bool customInput( ImGuiID id,
                             StringView idString,
                             ScalarT& value,
                             ImGuiDataType dataType,
                             const ImVec2& size ) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 inputSize = ImVec2( size.x, size.y );

        // Convert screen coordinates for custom drawing
        ImVec2 screenPos = ImGui::GetCursorScreenPos();

        bool isFocused = id == ImGui::GetActiveID();
        bool hovered = ImGui::IsMouseHoveringRect( screenPos,
                                                   ImVec2( screenPos.x + inputSize.x, screenPos.y + inputSize.y ) );

        // const float32 horizontalPadding = ImGui::GetStyle().FramePadding.x;
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Render border
        float32 frameRounding = style.FrameRounding;
        float32 borderSize = style.FrameBorderSize;
        if( style.FrameBorderSize > 0.0f ) {
            uint32_t borderColor = hovered     ? ImGui::GetColorU32( ImGuiCol_ButtonHovered )
                                   : isFocused ? ImGui::GetColorU32( ImGuiCol_ButtonActive )
                                               : ImGui::GetColorU32( ImGuiCol_Border );
            ImVec2 endPos( screenPos.x + inputSize.x, screenPos.y + inputSize.y );
            drawList->PathRect( screenPos + ImVec2( 0.50f, 0.50f ),
                                endPos - ImVec2( 0.50f, 0.50f ),
                                frameRounding,
                                ImDrawFlags_RoundCornersRight );
            drawList->PathStroke( borderColor, ImDrawFlags_None, borderSize );
        }

        // Render background
        uint32_t bgColor = ImGui::GetColorU32( ImGui::GetStyleColorVec4( ImGuiCol_FrameBg ) );
        if( hovered ) {
            bgColor = ImGui::GetColorU32( ImGui::GetStyleColorVec4( ImGuiCol_FrameBgHovered ) );
        }

        ImVec2 bgPos = ImVec2( screenPos.x, screenPos.y + borderSize );
        ImVec2 bgSize = ImVec2( inputSize.x - borderSize, inputSize.y - 2 * borderSize );

        drawList->AddRectFilled( bgPos,
                                 ImVec2( bgPos.x + bgSize.x, bgPos.y + bgSize.y ),
                                 bgColor,
                                 frameRounding - borderSize,
                                 ImDrawFlags_RoundCornersRight );

        StringView format = "{}";
        if constexpr( std::is_floating_point_v< ScalarT > ) {
            if( isZero( value, ScalarT( 0.001 ) ) ) {
                value = 0.0f;
                format = "{:.1f}";
            } else if( isEqual( std::floor( value ), value ) && ( isFocused == false ) ) {
                format = "{:.1f}";
            } else {
                format = "{:2.6g}";
            }
        }

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll;
        ImGui::SetNextItemWidth( inputSize.x );

        // Render the input field
        ScopedImGuiColor scopedColorOverride{
            { ImGuiCol_FrameBg, 0x00000000 },
            { ImGuiCol_FrameBgHovered, 0x00000000 },
            { ImGuiCol_FrameBgActive, 0x00000000 },
            { ImGuiCol_Border, 0x00000000 },
        };

        bool changed = drawScalarInput( idString.data(), dataType, value, nullptr, nullptr, format.data(), flags );
        return changed;
    }
};
} // namespace onyx::ui
#endif
