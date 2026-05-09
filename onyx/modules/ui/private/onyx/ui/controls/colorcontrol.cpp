#include <onyx/ui/controls/colorcontrol.h>

#include <onyx/platforms/windows/platform.h>
#include <onyx/ui/controls/vectorcontrol.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>

#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::ui {
namespace {
//// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb uint , from Foley & van Dam p593
//// also http://en.wikipedia.org/wiki/HSL_and_HSV
// uint32_t ColorConvertHSVtoBGR(float h, float s, float v)
//{
//     auto ConvertToU32 = [](float value) { return static_cast<uint32_t>(value * 255.0f + 0.5f); };

//    if (s == 0.0f)
//    {
//        // gray
//        uint32_t colorValue = ConvertToU32(v);
//        return colorValue << 16 | colorValue << 8 | colorValue;
//    }

//    h = ImFmod(h, 1.0f) / (60.0f / 360.0f);
//    int   i = (int)h;
//    float f = h - (float)i;
//    float p = v * (1.0f - s);
//    float q = v * (1.0f - s * f);
//    float t = v * (1.0f - s * (1.0f - f));

//    switch (i)
//    {
//    case 0: return (255 << 24) | ConvertToU32(v) | ConvertToU32(t) << 8 | ConvertToU32(p) << 16;
//    case 1: return (255 << 24) |ConvertToU32(q) | ConvertToU32(v) << 8 | ConvertToU32(p) << 16;
//    case 2: return (255 << 24) |ConvertToU32(p) | ConvertToU32(v) << 8 | ConvertToU32(t) << 16;
//    case 3: return (255 << 24) |ConvertToU32(p) | ConvertToU32(q) << 8 | ConvertToU32(v) << 16;
//    case 4: return (255 << 24) |ConvertToU32(t) | ConvertToU32(p) << 8 | ConvertToU32(v) << 16;
//    case 5: default: return (255 << 24) | ConvertToU32(v) | ConvertToU32(p) << 8 | ConvertToU32(q) << 16;
//    }
//}

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorEdit( StringView id, VectorT& outColor, ImGuiColorEditFlags flags );

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorPicker( StringView label, VectorT& outColor, ImGuiColorEditFlags flags );

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorPicker( StringView label, VectorT& outColor, ImGuiColorEditFlags flags, const ImVec4* refCol );

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
void colorEditRestoreH( const VectorT& col, typename VectorT::ScalarT& h ) {
    ::ImGuiContext& g = *GImGui;
    IM_ASSERT( g.ColorEditCurrentID != 0 );
    if( g.ColorEditSavedID != g.ColorEditCurrentID ||
        g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32( ImVec4( col.X, col.Y, col.Z, 0 ) ) )
        return;
    h = g.ColorEditSavedHue;
}

// ColorEdit supports RGB and HSV inputs. In case of RGB input resulting color may have undefined hue and/or saturation.
// Since widget displays both RGB and HSV values we must preserve hue and saturation to prevent these values resetting.
template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
void colorEditRestoreHs( const VectorT& col,
                         typename VectorT::ScalarT& h,
                         typename VectorT::ScalarT& s,
                         typename VectorT::ScalarT& v ) {
    ::ImGuiContext& g = *GImGui;
    IM_ASSERT( g.ColorEditCurrentID != 0 );
    if( g.ColorEditSavedID != g.ColorEditCurrentID ||
        g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32( ImVec4( col.X, col.Y, col.Z, 0 ) ) )
        return;

    // When S == 0, H is undefined.
    // When H == 1 it wraps around to 0.
    if( s == 0.0f || ( h == 0.0f && g.ColorEditSavedHue == 1 ) )
        h = g.ColorEditSavedHue;

    // When V == 0, S is undefined.
    if( v == 0.0f )
        s = g.ColorEditSavedSat;
}

void renderArrowsForVerticalBar( ImDrawList* drawList, ImVec2 pos, ImVec2 halfSz, float barW, float alpha ) {
    ImU32 alpha8 = IM_F32_TO_INT8_SAT( alpha );
    ImGui::RenderArrowPointingAt( drawList,
                                  ImVec2( pos.x + halfSz.x + 1, pos.y ),
                                  ImVec2( halfSz.x + 2, halfSz.y + 1 ),
                                  ImGuiDir_Right,
                                  IM_COL32( 0, 0, 0, alpha8 ) );
    ImGui::RenderArrowPointingAt( drawList,
                                  ImVec2( pos.x + halfSz.x, pos.y ),
                                  halfSz,
                                  ImGuiDir_Right,
                                  IM_COL32( 255, 255, 255, alpha8 ) );
    ImGui::RenderArrowPointingAt( drawList,
                                  ImVec2( pos.x + barW - halfSz.x - 1, pos.y ),
                                  ImVec2( halfSz.x + 2, halfSz.y + 1 ),
                                  ImGuiDir_Left,
                                  IM_COL32( 0, 0, 0, alpha8 ) );
    ImGui::RenderArrowPointingAt( drawList,
                                  ImVec2( pos.x + barW - halfSz.x, pos.y ),
                                  halfSz,
                                  ImGuiDir_Left,
                                  IM_COL32( 255, 255, 255, alpha8 ) );
}

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
void colorPickerOptionsPopup( const VectorT& refCol, ImGuiColorEditFlags flags ) {
    bool allowOptPicker = !( flags & ImGuiColorEditFlags_PickerMask_ );
    bool allowOptAlphaBar = !( flags & ImGuiColorEditFlags_NoAlpha ) && !( flags & ImGuiColorEditFlags_AlphaBar );
    if( ( !allowOptPicker && !allowOptAlphaBar ) || !ImGui::BeginPopup( "context" ) )
        return;

    ::ImGuiContext& g = *GImGui;
    ImGui::PushItemFlag( ImGuiItemFlags_NoMarkEdited, true );
    if( allowOptPicker ) {
        ImVec2 pickerSize( g.FontSize * 8,
                           ImMax( g.FontSize * 8 - ( ImGui::GetFrameHeight() + g.Style.ItemInnerSpacing.x ),
                                  1.0f ) ); // FIXME: Picker size copied from main picker function
        ImGui::PushItemWidth( pickerSize.x );
        for( int pickerType = 0; pickerType < 2; pickerType++ ) {
            // Draw small/thumbnail version of each picker type (over an invisible button for selection)
            if( pickerType > 0 )
                ImGui::Separator();
            ImGui::PushID( pickerType );
            ImGuiColorEditFlags pickerFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions |
                                              ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview |
                                              ( flags & ImGuiColorEditFlags_NoAlpha );
            if( pickerType == 0 )
                pickerFlags |= ImGuiColorEditFlags_PickerHueBar;
            if( pickerType == 1 )
                pickerFlags |= ImGuiColorEditFlags_PickerHueWheel;
            ImVec2 backupPos = ImGui::GetCursorScreenPos();
            if( ImGui::Selectable( "##selectable", false, 0,
                                   pickerSize ) ) // By default, Selectable() is closing popup
                g.ColorEditOptions = ( g.ColorEditOptions & ~ImGuiColorEditFlags_PickerMask_ ) |
                                     ( pickerFlags & ImGuiColorEditFlags_PickerMask_ );
            ImGui::SetCursorScreenPos( backupPos );
            VectorT previewingRefCol = refCol;
            colorPicker( "##previewing_picker", previewingRefCol, pickerFlags );
            ImGui::PopID();
        }
        ImGui::PopItemWidth();
    }
    if( allowOptAlphaBar ) {
        if( allowOptPicker )
            ImGui::Separator();
        ImGui::CheckboxFlags( "Alpha Bar", &g.ColorEditOptions, ImGuiColorEditFlags_AlphaBar );
    }
    ImGui::PopItemFlag();
    ImGui::EndPopup();
}

template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorPicker( StringView label, VectorT& outColor, ImGuiColorEditFlags flags ) {
    return colorPicker( label, outColor, flags, nullptr );
}

// Note: Templated version of ImGui::ColorPicker4().
// FIXME: we adjust the big color square height based on item width, which may cause a flickering feedback loop (if
// automatic height makes a vertical scrollbar appears, affecting automatic width..)
// FIXME: this is trying to be aware of style.Alpha but not fully correct. Also, the color wheel will have overlapping
// glitches with (style.Alpha < 1.0)
template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorPicker( StringView label, VectorT& outColor, ImGuiColorEditFlags flags, const ImVec4* refCol ) {
    constexpr bool HasAlpha = IsVector4< VectorT >;
    using ScalarT = typename VectorT::ScalarT;

    ::ImGuiContext& imguiContext = *GImGui;
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return false;

    ImDrawList* drawList = window->DrawList;
    ImGuiStyle& style = imguiContext.Style;
    ImGuiIO& io = imguiContext.IO;

    const float width = ImGui::CalcItemWidth();
    const bool isReadonly = ( ( imguiContext.NextItemData.ItemFlags | imguiContext.CurrentItemFlags ) &
                              ImGuiItemFlags_ReadOnly ) != 0;
    imguiContext.NextItemData.ClearFlags();

    ScopedImGuiId scopedId( label );
    const bool setCurrentColorEditId = ( imguiContext.ColorEditCurrentID == 0 );
    if( setCurrentColorEditId )
        imguiContext.ColorEditCurrentID = window->IDStack.back();
    ImGui::BeginGroup();

    if( !( flags & ImGuiColorEditFlags_NoSidePreview ) )
        flags |= ImGuiColorEditFlags_NoSmallPreview;

    // Context menu: display and store options.
    if( !( flags & ImGuiColorEditFlags_NoOptions ) )
        colorPickerOptionsPopup( outColor, flags );

    // Read stored options
    if( !( flags & ImGuiColorEditFlags_PickerMask_ ) )
        flags |= ( ( imguiContext.ColorEditOptions & ImGuiColorEditFlags_PickerMask_ )
                       ? imguiContext.ColorEditOptions
                       : ImGuiColorEditFlags_DefaultOptions_ ) &
                 ImGuiColorEditFlags_PickerMask_;
    if( !( flags & ImGuiColorEditFlags_InputMask_ ) )
        flags |= ( ( imguiContext.ColorEditOptions & ImGuiColorEditFlags_InputMask_ )
                       ? imguiContext.ColorEditOptions
                       : ImGuiColorEditFlags_DefaultOptions_ ) &
                 ImGuiColorEditFlags_InputMask_;
    IM_ASSERT( ImIsPowerOfTwo( flags & ImGuiColorEditFlags_PickerMask_ ) ); // Check that only 1 is selected
    IM_ASSERT( ImIsPowerOfTwo( flags & ImGuiColorEditFlags_InputMask_ ) );  // Check that only 1 is selected
    if( !( flags & ImGuiColorEditFlags_NoOptions ) )
        flags |= ( imguiContext.ColorEditOptions & ImGuiColorEditFlags_AlphaBar );

    // Setup
    bool alphaBar = ( flags & ImGuiColorEditFlags_AlphaBar ) && !( flags & ImGuiColorEditFlags_NoAlpha );
    ImVec2 pickerPos = window->DC.CursorPos;
    float squareSz = ImGui::GetFrameHeight();
    float barsWidth = squareSz; // Arbitrary smallish width of Hue/Alpha picking bars
    float svPickerSize = ImMax( barsWidth * 1,
                                width - numericCast< float32 >( alphaBar ? 2 : 1 ) *
                                            ( barsWidth + style.ItemInnerSpacing.x ) ); // Saturation/Value picking box
    float bar0PosX = pickerPos.x + svPickerSize + style.ItemInnerSpacing.x;
    float bar1PosX = bar0PosX + barsWidth + style.ItemInnerSpacing.x;
    float barsTrianglesHalfSz = IM_TRUNC( barsWidth * 0.20f );

    VectorT backupInitialCol = outColor;

    float wheelThickness = svPickerSize * 0.08f;
    float wheelROuter = svPickerSize * 0.50f;
    float wheelRInner = wheelROuter - wheelThickness;
    ImVec2 wheelCenter( pickerPos.x + ( svPickerSize + barsWidth ) * 0.5f, pickerPos.y + svPickerSize * 0.5f );

    // Note: the triangle is displayed rotated with triangle_pa pointing to Hue, but most coordinates stays unrotated
    // for logic.
    float triangleR = wheelRInner - numericCast< float32 >( std::lround( svPickerSize * 0.027f ) );
    ImVec2 trianglePa = ImVec2( triangleR, 0.0f );                           // Hue point.
    ImVec2 trianglePb = ImVec2( triangleR * -0.5f, triangleR * -0.866025f ); // Black point.
    ImVec2 trianglePc = ImVec2( triangleR * -0.5f, triangleR * +0.866025f ); // White point.

    float h = outColor.X, s = outColor.Y, v = outColor.Z;
    float r = outColor.X, g = outColor.Y, b = outColor.Z;
    if( flags & ImGuiColorEditFlags_InputRGB ) {
        // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
        ImGui::ColorConvertRGBtoHSV( r, g, b, h, s, v );
        colorEditRestoreHs( outColor, h, s, v );
    } else if( flags & ImGuiColorEditFlags_InputHSV ) {
        ImGui::ColorConvertHSVtoRGB( h, s, v, r, g, b );
    }

    bool valueChanged = false, valueChangedH = false, valueChangedSv = false;

    ImGui::PushItemFlag( ImGuiItemFlags_NoNav, true );
    if( flags & ImGuiColorEditFlags_PickerHueWheel ) {
        // Hue wheel + SV triangle logic
        ImGui::InvisibleButton( "hsv", ImVec2( svPickerSize + style.ItemInnerSpacing.x + barsWidth, svPickerSize ) );
        if( ImGui::IsItemActive() && !isReadonly ) {
            ImVec2 initialOff = io.MouseClickedPos[ 0 ] - wheelCenter;
            ImVec2 currentOff = io.MousePos - wheelCenter;
            float initialDist2 = ImLengthSqr( initialOff );
            if( initialDist2 >= ( wheelRInner - 1 ) * ( wheelRInner - 1 ) &&
                initialDist2 <= ( wheelROuter + 1 ) * ( wheelROuter + 1 ) ) {
                // Interactive with Hue wheel
                h = ImAtan2( currentOff.y, currentOff.x ) / IM_PI * 0.5f;
                if( h < 0.0f )
                    h += 1.0f;
                valueChanged = valueChangedH = true;
            }
            float cosHueAngle = ImCos( -h * 2.0f * IM_PI );
            float sinHueAngle = ImSin( -h * 2.0f * IM_PI );
            if( ImTriangleContainsPoint( trianglePa,
                                         trianglePb,
                                         trianglePc,
                                         ImRotate( initialOff, cosHueAngle, sinHueAngle ) ) ) {
                // Interacting with SV triangle
                ImVec2 currentOffUnrotated = ImRotate( currentOff, cosHueAngle, sinHueAngle );
                if( !ImTriangleContainsPoint( trianglePa, trianglePb, trianglePc, currentOffUnrotated ) )
                    currentOffUnrotated = ImTriangleClosestPoint( trianglePa,
                                                                  trianglePb,
                                                                  trianglePc,
                                                                  currentOffUnrotated );
                float uu, vv, ww;
                ImTriangleBarycentricCoords( trianglePa, trianglePb, trianglePc, currentOffUnrotated, uu, vv, ww );
                v = ImClamp( 1.0f - vv, 0.0001f, 1.0f );
                s = ImClamp( uu / v, 0.0001f, 1.0f );
                valueChanged = valueChangedSv = true;
            }
        }
        if( !( flags & ImGuiColorEditFlags_NoOptions ) )
            ImGui::OpenPopupOnItemClick( "context", ImGuiPopupFlags_MouseButtonRight );
    } else if( flags & ImGuiColorEditFlags_PickerHueBar ) {
        // SV rectangle logic
        ImGui::InvisibleButton( "sv", ImVec2( svPickerSize, svPickerSize ) );
        if( ImGui::IsItemActive() && !isReadonly ) {
            s = ImSaturate( ( io.MousePos.x - pickerPos.x ) / ( svPickerSize - 1 ) );
            v = 1.0f - ImSaturate( ( io.MousePos.y - pickerPos.y ) / ( svPickerSize - 1 ) );
            colorEditRestoreH( outColor, h ); // Greatly reduces hue jitter and reset to 0 when hue == 255 and color is
                                              // rapidly modified using SV square.
            valueChanged = valueChangedSv = true;
        }
        if( !( flags & ImGuiColorEditFlags_NoOptions ) )
            ImGui::OpenPopupOnItemClick( "context", ImGuiPopupFlags_MouseButtonRight );

        // Hue bar logic
        ImGui::SetCursorScreenPos( ImVec2( bar0PosX, pickerPos.y ) );
        ImGui::InvisibleButton( "hue", ImVec2( barsWidth, svPickerSize ) );
        if( ImGui::IsItemActive() && !isReadonly ) {
            h = ImSaturate( ( io.MousePos.y - pickerPos.y ) / ( svPickerSize - 1 ) );
            valueChanged = valueChangedH = true;
        }
    }

    // Alpha bar logic
    if constexpr( HasAlpha ) {
        if( alphaBar ) {
            ImGui::SetCursorScreenPos( ImVec2( bar1PosX, pickerPos.y ) );
            ImGui::InvisibleButton( "alpha", ImVec2( barsWidth, svPickerSize ) );
            if( ImGui::IsItemActive() ) {
                outColor.W = 1.0f - ImSaturate( ( io.MousePos.y - pickerPos.y ) / ( svPickerSize - 1 ) );
                valueChanged = true;
            }
        }
    }

    ImGui::PopItemFlag(); // ImGuiItemFlags_NoNav

    if( !( flags & ImGuiColorEditFlags_NoSidePreview ) ) {
        ImGui::SameLine( 0, style.ItemInnerSpacing.x );
        ImGui::BeginGroup();
    }

    if( !( flags & ImGuiColorEditFlags_NoLabel ) ) {
        const char* labelDisplayEnd = ImGui::FindRenderedTextEnd( label.data() );
        if( label != labelDisplayEnd ) {
            if( ( flags & ImGuiColorEditFlags_NoSidePreview ) )
                ImGui::SameLine( 0, style.ItemInnerSpacing.x );
            ImGui::TextEx( label.data(), labelDisplayEnd );
        }
    }

    if( !( flags & ImGuiColorEditFlags_NoSidePreview ) ) {
        ImGui::PushItemFlag( ImGuiItemFlags_NoNavDefaultFocus, true );
        ImVec4 colV4( static_cast< float32 >( outColor.X ),
                      static_cast< float32 >( outColor.Y ),
                      static_cast< float32 >( outColor.Z ),
                      1.0f );

        if constexpr( HasAlpha ) {
            colV4.w = ( flags & ImGuiColorEditFlags_NoAlpha ) ? 1.0f : static_cast< float32 >( outColor.W );
        }

        if( ( flags & ImGuiColorEditFlags_NoLabel ) )
            ImGui::Text( "Current" );

        ImGuiColorEditFlags subFlagsToForward = ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR |
                                                ImGuiColorEditFlags_AlphaPreview |
                                                ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoTooltip;
        ImGui::ColorButton( "##current", colV4, ( flags & subFlagsToForward ), ImVec2( squareSz * 3, squareSz * 2 ) );
        if( refCol != nullptr ) {
            ImGui::Text( "Original" );
            ImVec4 refColV4( refCol->x,
                             refCol->y,
                             refCol->z,
                             ( flags & ImGuiColorEditFlags_NoAlpha ) ? 1.0f : refCol->w );
            if( ImGui::ColorButton( "##original",
                                    refColV4,
                                    ( flags & subFlagsToForward ),
                                    ImVec2( squareSz * 3, squareSz * 2 ) ) ) {
                outColor.X = static_cast< ScalarT >( colV4.x );
                outColor.Y = static_cast< ScalarT >( colV4.y );
                outColor.Z = static_cast< ScalarT >( colV4.z );
                if constexpr( HasAlpha ) {
                    outColor.W = static_cast< ScalarT >( colV4.w );
                }
                valueChanged = true;
            }
        }

#if ONYX_IS_WINDOWS
        ImGuiID colorPickStateId = ImGui::GetID( "isColorPickActive" );
        ImGuiStorage* storage = ImGui::GetStateStorage();
        bool& colorPickState = *storage->GetBoolRef( colorPickStateId, false );
        // ImGuiID buttonID = ImGui::GetID("Pick");
        if( ImGui::Button( "Pick" ) ) {
            colorPickState = true;
            g_UiContext.InputSystem->EnableSystemMouseCapture( true );
        }

        if( colorPickState ) {
            if( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) ) {
                constexpr float32 U8ToF32 = 1.0f / 255.0f;
                Vector3u8 pixelColor = platform::GetPixelColorAtMousePosition();
                Vector3f32 saturatedPixelColor( pixelColor.X * U8ToF32,
                                                pixelColor.Y * U8ToF32,
                                                pixelColor.Z * U8ToF32 );

                if( outColor != saturatedPixelColor ) {
                    outColor = saturatedPixelColor;
                    value_changed = true;
                }

                colorPickState = false;
                g_UiContext.InputSystem->EnableSystemMouseCapture( false );

                io.MouseClicked[ 0 ] = false; // consume mouse click to not lose focus
                io.MouseDown[ 0 ] = false;
            }
        }
#endif
        ImGui::PopItemFlag();
        ImGui::EndGroup();
    }

    // Convert back color to RGB
    if( valueChangedH || valueChangedSv ) {
        if( flags & ImGuiColorEditFlags_InputRGB ) {
            ImGui::ColorConvertHSVtoRGB( h, s, v, outColor.X, outColor.Y, outColor.Z );
            imguiContext.ColorEditSavedHue = h;
            imguiContext.ColorEditSavedSat = s;
            imguiContext.ColorEditSavedID = imguiContext.ColorEditCurrentID;
            imguiContext.ColorEditSavedColor = ImGui::ColorConvertFloat4ToU32(
                ImVec4( outColor.X, outColor.Y, outColor.Z, 0 ) );
        } else if( flags & ImGuiColorEditFlags_InputHSV ) {
            outColor.X = static_cast< ScalarT >( h );
            outColor.X = static_cast< ScalarT >( s );
            outColor.X = static_cast< ScalarT >( v );
        }
    }

    // R,G,B and H,S,V slider color editor
    bool valueChangedFixHueWrap = false;
    if( ( flags & ImGuiColorEditFlags_NoInputs ) == 0 ) {
        ImGui::PushItemWidth( ( alphaBar ? bar1PosX : bar0PosX ) + barsWidth - pickerPos.x );
        ImGuiColorEditFlags subFlagsToForward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_InputMask_ |
                                                ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha |
                                                ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoTooltip |
                                                ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview |
                                                ImGuiColorEditFlags_AlphaPreviewHalf;
        ImGuiColorEditFlags subFlags = ( flags & subFlagsToForward ) | ImGuiColorEditFlags_NoPicker;
        if( flags & ImGuiColorEditFlags_DisplayRGB || ( flags & ImGuiColorEditFlags_DisplayMask_ ) == 0 )
            if( colorEdit( "##rgb", outColor, subFlags | ImGuiColorEditFlags_DisplayRGB ) ) {
                // FIXME: Hackily differentiating using the DragInt (ActiveId != 0 && !ActiveIdAllowOverlap) vs. using
                // the InputText or DropTarget. For the later we don't want to run the hue-wrap canceling code. If you
                // are well versed in HSV picker please provide your input! (See #2050)
                valueChangedFixHueWrap = ( imguiContext.ActiveId != 0 && !imguiContext.ActiveIdAllowOverlap );
                valueChanged = true;
            }
        if( flags & ImGuiColorEditFlags_DisplayHSV || ( flags & ImGuiColorEditFlags_DisplayMask_ ) == 0 )
            valueChanged |= colorEdit( "##hsv", outColor, subFlags | ImGuiColorEditFlags_DisplayHSV );
        if( flags & ImGuiColorEditFlags_DisplayHex || ( flags & ImGuiColorEditFlags_DisplayMask_ ) == 0 )
            valueChanged |= colorEdit( "##hex", outColor, subFlags | ImGuiColorEditFlags_DisplayHex );
        ImGui::PopItemWidth();
    }

    // Try to cancel hue wrap (after ColorEdit4 call), if any
    if( valueChangedFixHueWrap && ( flags & ImGuiColorEditFlags_InputRGB ) ) {
        ScalarT newH, newS, newV;
        ImGui::ColorConvertRGBtoHSV( outColor.X, outColor.Y, outColor.Z, newH, newS, newV );
        if( newH <= 0 && h > 0 ) {
            if( newV <= 0 && v != newV )
                ImGui::ColorConvertHSVtoRGB( h, s, newV <= 0 ? v * 0.5f : newV, outColor.X, outColor.Y, outColor.Z );
            else if( newS <= 0 )
                ImGui::ColorConvertHSVtoRGB( h, newS <= 0 ? s * 0.5f : newS, newV, outColor.X, outColor.Y, outColor.Z );
        }
    }

    if( valueChanged ) {
        if( flags & ImGuiColorEditFlags_InputRGB ) {
            r = outColor.X;
            g = outColor.Y;
            b = outColor.Z;
            ImGui::ColorConvertRGBtoHSV( r, g, b, h, s, v );
            colorEditRestoreHs( outColor, h, s, v ); // Fix local Hue as display below will use it immediately.
        } else if( flags & ImGuiColorEditFlags_InputHSV ) {
            h = outColor.X;
            s = outColor.Y;
            v = outColor.Z;
            ImGui::ColorConvertHSVtoRGB( h, s, v, r, g, b );
        }
    }

    const int styleAlpha8 = IM_F32_TO_INT8_SAT( style.Alpha );
    const ImU32 colBlack = IM_COL32( 0, 0, 0, styleAlpha8 );
    const ImU32 colWhite = IM_COL32( 255, 255, 255, styleAlpha8 );
    const ImU32 colMidgrey = IM_COL32( 128, 128, 128, styleAlpha8 );
    const ImU32 colHues[ 6 + 1 ] = { IM_COL32( 255, 0, 0, styleAlpha8 ),
                                     IM_COL32( 255, 255, 0, styleAlpha8 ),
                                     IM_COL32( 0, 255, 0, styleAlpha8 ),
                                     IM_COL32( 0, 255, 255, styleAlpha8 ),
                                     IM_COL32( 0, 0, 255, styleAlpha8 ),
                                     IM_COL32( 255, 0, 255, styleAlpha8 ),
                                     IM_COL32( 255, 0, 0, styleAlpha8 ) };

    ImVec4 hueColorF( 1, 1, 1, style.Alpha );
    ImGui::ColorConvertHSVtoRGB( h, ScalarT( 1 ), ScalarT( 1 ), hueColorF.x, hueColorF.y, hueColorF.z );
    ImU32 hueColor32 = ImGui::ColorConvertFloat4ToU32( hueColorF );
    ImU32 userCol32StripedOfAlpha = ImGui::ColorConvertFloat4ToU32(
        ImVec4( r, g, b, style.Alpha ) ); // Important: this is still including the main rendering/style alpha!!

    ImVec2 svCursorPos;

    if( flags & ImGuiColorEditFlags_PickerHueWheel ) {
        // Render Hue Wheel
        const float aeps = 0.5f / wheelROuter; // Half a pixel arc length in radians (2pi cancels out).
        const int segmentPerArc = ImMax( 4, (int)wheelROuter / 12 );
        for( int n = 0; n < 6; n++ ) {
            const float32 a0 = numericCast< float32 >( n ) / 6.0f * 2.0f * IM_PI - aeps;
            const float32 a1 = numericCast< float32 >( n + 1 ) / 6.0f * 2.0f * IM_PI + aeps;
            const int vertStartIdx = drawList->VtxBuffer.Size;
            drawList->PathArcTo( wheelCenter, ( wheelRInner + wheelROuter ) * 0.5f, a0, a1, segmentPerArc );
            drawList->PathStroke( colWhite, 0, wheelThickness );
            const int vertEndIdx = drawList->VtxBuffer.Size;

            // Paint colors over existing vertices
            ImVec2 gradientP0( wheelCenter.x + ImCos( a0 ) * wheelRInner, wheelCenter.y + ImSin( a0 ) * wheelRInner );
            ImVec2 gradientP1( wheelCenter.x + ImCos( a1 ) * wheelRInner, wheelCenter.y + ImSin( a1 ) * wheelRInner );
            ImGui::ShadeVertsLinearColorGradientKeepAlpha( drawList,
                                                           vertStartIdx,
                                                           vertEndIdx,
                                                           gradientP0,
                                                           gradientP1,
                                                           colHues[ n ],
                                                           colHues[ n + 1 ] );
        }

        // Render Cursor + preview on Hue Wheel
        float cosHueAngle = ImCos( h * 2.0f * IM_PI );
        float sinHueAngle = ImSin( h * 2.0f * IM_PI );
        ImVec2 hueCursorPos( wheelCenter.x + cosHueAngle * ( wheelRInner + wheelROuter ) * 0.5f,
                             wheelCenter.y + sinHueAngle * ( wheelRInner + wheelROuter ) * 0.5f );
        float hueCursorRad = valueChangedH ? wheelThickness * 0.65f : wheelThickness * 0.55f;
        int hueCursorSegments = drawList->_CalcCircleAutoSegmentCount(
            hueCursorRad ); // Lock segment count so the +1 one matches others.
        drawList->AddCircleFilled( hueCursorPos, hueCursorRad, hueColor32, hueCursorSegments );
        drawList->AddCircle( hueCursorPos, hueCursorRad + 1, colMidgrey, hueCursorSegments );
        drawList->AddCircle( hueCursorPos, hueCursorRad, colWhite, hueCursorSegments );

        // Render SV triangle (rotated according to hue)
        ImVec2 tra = wheelCenter + ImRotate( trianglePa, cosHueAngle, sinHueAngle );
        ImVec2 trb = wheelCenter + ImRotate( trianglePb, cosHueAngle, sinHueAngle );
        ImVec2 trc = wheelCenter + ImRotate( trianglePc, cosHueAngle, sinHueAngle );
        ImVec2 uvWhite = ImGui::GetFontTexUvWhitePixel();
        drawList->PrimReserve( 3, 3 );
        drawList->PrimVtx( tra, uvWhite, hueColor32 );
        drawList->PrimVtx( trb, uvWhite, colBlack );
        drawList->PrimVtx( trc, uvWhite, colWhite );
        drawList->AddTriangle( tra, trb, trc, colMidgrey, 1.5f );
        svCursorPos = ImLerp( ImLerp( trc, tra, ImSaturate( s ) ), trb, ImSaturate( 1 - v ) );
    } else if( flags & ImGuiColorEditFlags_PickerHueBar ) {
        // Render SV Square
        drawList->AddRectFilledMultiColor( pickerPos,
                                           pickerPos + ImVec2( svPickerSize, svPickerSize ),
                                           colWhite,
                                           hueColor32,
                                           hueColor32,
                                           colWhite );
        drawList->AddRectFilledMultiColor( pickerPos,
                                           pickerPos + ImVec2( svPickerSize, svPickerSize ),
                                           0,
                                           0,
                                           colBlack,
                                           colBlack );
        ImGui::RenderFrameBorder( pickerPos, pickerPos + ImVec2( svPickerSize, svPickerSize ), 0.0f );
        svCursorPos.x = ImClamp( numericCast< float32 >( std::lround( pickerPos.x + ImSaturate( s ) * svPickerSize ) ),
                                 pickerPos.x + 2,
                                 pickerPos.x + svPickerSize - 2 ); // Sneakily prevent the circle to stick out too much
        svCursorPos.y = ImClamp(
            numericCast< float32 >( std::lround( pickerPos.y + ImSaturate( 1 - v ) * svPickerSize ) ),
            pickerPos.y + 2,
            pickerPos.y + svPickerSize - 2 );

        // Render Hue Bar
        for( int i = 0; i < 6; ++i )
            drawList->AddRectFilledMultiColor(
                ImVec2( bar0PosX, pickerPos.y + numericCast< float32 >( i ) * ( svPickerSize / 6 ) ),
                ImVec2( bar0PosX + barsWidth, pickerPos.y + numericCast< float32 >( i + 1 ) * ( svPickerSize / 6 ) ),
                colHues[ i ],
                colHues[ i ],
                colHues[ i + 1 ],
                colHues[ i + 1 ] );
        float32 bar0LineY = numericCast< float32 >( std::lround( pickerPos.y + h * svPickerSize ) );
        ImGui::RenderFrameBorder( ImVec2( bar0PosX, pickerPos.y ),
                                  ImVec2( bar0PosX + barsWidth, pickerPos.y + svPickerSize ),
                                  0.0f );
        renderArrowsForVerticalBar( drawList,
                                    ImVec2( bar0PosX - 1, bar0LineY ),
                                    ImVec2( barsTrianglesHalfSz + 1, barsTrianglesHalfSz ),
                                    barsWidth + 2.0f,
                                    style.Alpha );
    }

    // Render cursor/preview circle (clamp S/V within 0..1 range because floating points colors may lead HSV values to
    // be out of range)
    float svCursorRad = valueChangedSv ? wheelThickness * 0.55f : wheelThickness * 0.40f;
    int svCursorSegments = drawList->_CalcCircleAutoSegmentCount(
        svCursorRad ); // Lock segment count so the +1 one matches others.
    drawList->AddCircleFilled( svCursorPos, svCursorRad, userCol32StripedOfAlpha, svCursorSegments );
    drawList->AddCircle( svCursorPos, svCursorRad + 1, colMidgrey, svCursorSegments );
    drawList->AddCircle( svCursorPos, svCursorRad, colWhite, svCursorSegments );

    // Render alpha bar
    if constexpr( HasAlpha ) {
        if( alphaBar ) {
            float alpha = ImSaturate( outColor.W );
            ImRect bar1Bb( bar1PosX, pickerPos.y, bar1PosX + barsWidth, pickerPos.y + svPickerSize );
            ImGui::RenderColorRectWithAlphaCheckerboard( drawList,
                                                         bar1Bb.Min,
                                                         bar1Bb.Max,
                                                         0,
                                                         bar1Bb.GetWidth() / 2.0f,
                                                         ImVec2( 0.0f, 0.0f ) );
            drawList->AddRectFilledMultiColor( bar1Bb.Min,
                                               bar1Bb.Max,
                                               userCol32StripedOfAlpha,
                                               userCol32StripedOfAlpha,
                                               userCol32StripedOfAlpha & ~IM_COL32_A_MASK,
                                               userCol32StripedOfAlpha & ~IM_COL32_A_MASK );
            float32 bar1LineY = numericCast< float32 >( std::lround( pickerPos.y + ( 1.0f - alpha ) * svPickerSize ) );
            ImGui::RenderFrameBorder( bar1Bb.Min, bar1Bb.Max, 0.0f );
            renderArrowsForVerticalBar( drawList,
                                        ImVec2( bar1PosX - 1, bar1LineY ),
                                        ImVec2( barsTrianglesHalfSz + 1, barsTrianglesHalfSz ),
                                        barsWidth + 2.0f,
                                        style.Alpha );
        }
    }

    ImGui::EndGroup();

    if( valueChanged && backupInitialCol == outColor )
        valueChanged = false;

    if( valueChanged &&
        imguiContext.LastItemData.ID != 0 ) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        ImGui::MarkItemEdited( imguiContext.LastItemData.ID );

    if( setCurrentColorEditId )
        imguiContext.ColorEditCurrentID = 0;

    return valueChanged;
}

// Edit colors components (each component in 0.0f..1.0f range).
// See enum ImGuiColorEditFlags_ for available options. e.g. Only access 3 floats if ImGuiColorEditFlags_NoAlpha flag is
// set. With typical options: Left-click on color square to open color picker. Right-click to open option menu.
// CTRL-Click over input fields to edit them and TAB to go to next item.
template < typename VectorT > requires( IsVector3< VectorT > || IsVector4< VectorT > )
bool colorEdit( StringView id, VectorT& outColor, ImGuiColorEditFlags flags ) {
    constexpr bool HasAlpha = IsVector4< VectorT >;

    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    if( window->SkipItems )
        return false;

    ::ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const float squareSz = ImGui::GetFrameHeight();
    const char* labelDisplayEnd = ImGui::FindRenderedTextEnd( id.data() );
    float wFull = ImGui::CalcItemWidth();
    g.NextItemData.ClearFlags();

    bool valueChanged = false;
    bool valueChangedAsFloat = false;

    {
        ImGui::BeginGroup();
        ScopedImGuiId imguiScopeId( id );

        const bool setCurrentColorEditId = ( g.ColorEditCurrentID == 0 );
        if( setCurrentColorEditId )
            g.ColorEditCurrentID = window->IDStack.back();

        // If we're not showing any slider there's no point in doing any HSV conversions
        const ImGuiColorEditFlags flagsUntouched = flags;
        if( flags & ImGuiColorEditFlags_NoInputs )
            flags = ( flags & ( ~ImGuiColorEditFlags_DisplayMask_ ) ) | ImGuiColorEditFlags_DisplayRGB |
                    ImGuiColorEditFlags_NoOptions;

        // Context menu: display and modify options (before defaults are applied)
        if( !( flags & ImGuiColorEditFlags_NoOptions ) )
            ImGui::ColorEditOptionsPopup( &outColor.X, flags );

        // Read stored options
        if( !( flags & ImGuiColorEditFlags_DisplayMask_ ) )
            flags |= ( g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_ );
        if( !( flags & ImGuiColorEditFlags_DataTypeMask_ ) )
            flags |= ( g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_ );
        if( !( flags & ImGuiColorEditFlags_PickerMask_ ) )
            flags |= ( g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_ );
        if( !( flags & ImGuiColorEditFlags_InputMask_ ) )
            flags |= ( g.ColorEditOptions & ImGuiColorEditFlags_InputMask_ );
        flags |= ( g.ColorEditOptions & ~( ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ |
                                           ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ ) );
        IM_ASSERT( ImIsPowerOfTwo( flags & ImGuiColorEditFlags_DisplayMask_ ) ); // Check that only 1 is selected
        IM_ASSERT( ImIsPowerOfTwo( flags & ImGuiColorEditFlags_InputMask_ ) );   // Check that only 1 is selected

        const float wButton = ( flags & ImGuiColorEditFlags_NoSmallPreview ) ? 0.0f
                                                                             : ( squareSz + style.ItemInnerSpacing.x );
        const float wInputs = ImMax( wFull - wButton, 1.0f );
        wFull = wInputs + wButton;

        // Convert to the formats we need
        auto floatVector = [ & ]() {
            if constexpr( HasAlpha )
                return Vector4f32{ outColor.X, outColor.Y, outColor.Z, outColor.W };
            else
                return Vector3f32{ outColor.X, outColor.Y, outColor.Z };
        }();

        if( ( flags & ImGuiColorEditFlags_InputHSV ) && ( flags & ImGuiColorEditFlags_DisplayRGB ) )
            ImGui::ColorConvertHSVtoRGB( floatVector[ 0 ],
                                         floatVector[ 1 ],
                                         floatVector[ 2 ],
                                         floatVector[ 0 ],
                                         floatVector[ 1 ],
                                         floatVector[ 2 ] );
        else if( ( flags & ImGuiColorEditFlags_InputRGB ) && ( flags & ImGuiColorEditFlags_DisplayHSV ) ) {
            // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
            ImGui::ColorConvertRGBtoHSV( floatVector[ 0 ],
                                         floatVector[ 1 ],
                                         floatVector[ 2 ],
                                         floatVector[ 0 ],
                                         floatVector[ 1 ],
                                         floatVector[ 2 ] );
            colorEditRestoreHs( outColor, floatVector.X, floatVector.Y, floatVector.Z );
        }

        auto toRGB = []( float32 val ) {
            return static_cast< uint8_t >( std::lround( std::clamp( val, 0.0f, 1.0f ) * 255.0f ) );
        };
        auto intVector = [ & ]() {
            if constexpr( HasAlpha )
                return Vector4u8{ toRGB( floatVector[ 0 ] ),
                                  toRGB( floatVector[ 1 ] ),
                                  toRGB( floatVector[ 2 ] ),
                                  toRGB( floatVector[ 3 ] ) };
            else
                return Vector3u8{ toRGB( floatVector[ 0 ] ), toRGB( floatVector[ 1 ] ), toRGB( floatVector[ 2 ] ) };
        }();

        const ImVec2 pos = window->DC.CursorPos;
        const float inputsOffsetX = ( style.ColorButtonPosition == ImGuiDir_Left ) ? wButton : 0.0f;
        window->DC.CursorPos.x = pos.x + inputsOffsetX;

        ::ImGuiWindow* pickerActiveWindow = nullptr;
        if( ( flags & ( ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV ) ) != 0 &&
            ( flags & ImGuiColorEditFlags_NoInputs ) == 0 ) {
            {
                float32 componentInputSize = 50.0f;
                ScopedImGuiStyle styleOverride( ImGuiStyleVar_ItemSpacing, ImVec2( 10.0f, 0.0f ) );
                ImGui::BeginHorizontal( "##controls" );

                const bool isDisplayingRGB = enums::any( flags, ImGuiColorEditFlags_DisplayRGB );

                const StringView labelX = isDisplayingRGB ? "R" : "H";
                const StringView labelY = isDisplayingRGB ? "G" : "S";
                const StringView labelZ = isDisplayingRGB ? "B" : "V";
                const StringView labelW = "A";

                //                        float32 red, green, blue;

                // ImGui::ColorConvertHSVtoRGB(floatVector[0], 1.0f, 0.5f, red, green, blue);

                ImGuiSystem* imguiSystem = static_cast< ImGuiSystem* >( ImGui::GetIO().UserData );
                const Theme& theme = imguiSystem->getTheme();

                const uint32_t colorX = isDisplayingRGB ? theme.Colors.AxisX.toABGR() : theme.Colors.AxisW.toABGR();

                const uint32_t colorY = isDisplayingRGB ? theme.Colors.AxisY.toABGR() : theme.Colors.AxisW.toABGR();

                const uint32_t colorZ = isDisplayingRGB ? theme.Colors.AxisZ.toABGR() : theme.Colors.AxisW.toABGR();

                const uint32_t colorW = theme.Colors.AxisW.toABGR();

                if( flags & ImGuiColorEditFlags_Float ) {
                    // ImGui::BeginHorizontal("##vec3Inputs");

                    // TODO: show error tooltip or error effect on UI when clamping (red flash)
                    valueChanged |= VectorControl::vectorComponentInput( labelX,
                                                                         floatVector.X,
                                                                         ImGuiDataType_Float,
                                                                         componentInputSize,
                                                                         colorX );
                    valueChanged |= VectorControl::vectorComponentInput( labelY,
                                                                         floatVector.Y,
                                                                         ImGuiDataType_Float,
                                                                         componentInputSize,
                                                                         colorY );
                    valueChanged |= VectorControl::vectorComponentInput( labelZ,
                                                                         floatVector.Z,
                                                                         ImGuiDataType_Float,
                                                                         componentInputSize,
                                                                         colorZ );

                    if constexpr( HasAlpha ) {
                        valueChanged |= VectorControl::vectorComponentInput( labelW,
                                                                             floatVector.W,
                                                                             ImGuiDataType_Float,
                                                                             componentInputSize,
                                                                             colorW );
                    }
                } else {
                    valueChanged |= VectorControl::vectorComponentInput( labelX,
                                                                         intVector.X,
                                                                         ImGuiDataType_U8,
                                                                         componentInputSize,
                                                                         colorX );
                    valueChanged |= VectorControl::vectorComponentInput( labelY,
                                                                         intVector.Y,
                                                                         ImGuiDataType_U8,
                                                                         componentInputSize,
                                                                         colorY );
                    valueChanged |= VectorControl::vectorComponentInput( labelZ,
                                                                         intVector.Z,
                                                                         ImGuiDataType_U8,
                                                                         componentInputSize,
                                                                         colorZ );

                    if constexpr( HasAlpha ) {
                        valueChanged |= VectorControl::vectorComponentInput( labelW,
                                                                             intVector.W,
                                                                             ImGuiDataType_U8,
                                                                             componentInputSize,
                                                                             colorW );
                    }
                }
            }

            if( !( flags & ImGuiColorEditFlags_NoOptions ) )
                ImGui::OpenPopupOnItemClick( "context", ImGuiPopupFlags_MouseButtonRight );

            if( !( flags & ImGuiColorEditFlags_NoSmallPreview ) ) {
                ImVec4 colV4( outColor.X, outColor.Y, outColor.Z, 1.0f );

                if constexpr( HasAlpha ) {
                    colV4.w = outColor.W;
                }

                if( ImGui::ColorButton( "##ColorButton", colV4, flags ) ) {
                    if( !( flags & ImGuiColorEditFlags_NoPicker ) ) {
                        // Store current color and open a picker
                        g.ColorPickerRef = colV4;
                        ImGui::OpenPopup( "picker" );
                        ImGui::SetNextWindowPos( g.LastItemData.Rect.GetBL() + ImVec2( 0.0f, style.ItemSpacing.y ) );
                    }
                }

                if( !( flags & ImGuiColorEditFlags_NoOptions ) )
                    ImGui::OpenPopupOnItemClick( "context", ImGuiPopupFlags_MouseButtonRight );

                if( ImGui::BeginPopup( "picker", ImGuiWindowFlags_Popup ) ) {
                    if( g.CurrentWindow->BeginCount == 1 ) {
                        pickerActiveWindow = g.CurrentWindow;
                        if( id.data() != labelDisplayEnd ) {
                            ImGui::TextEx( id.data(), labelDisplayEnd );
                            ImGui::Spacing();
                        }
                        ImGuiColorEditFlags pickerFlagsToForward = ImGuiColorEditFlags_DataTypeMask_ |
                                                                   ImGuiColorEditFlags_PickerMask_ |
                                                                   ImGuiColorEditFlags_InputMask_ |
                                                                   ImGuiColorEditFlags_HDR |
                                                                   ImGuiColorEditFlags_NoAlpha |
                                                                   ImGuiColorEditFlags_AlphaBar;
                        ImGuiColorEditFlags pickerFlags = ( flagsUntouched & pickerFlagsToForward ) |
                                                          ImGuiColorEditFlags_DisplayMask_ |
                                                          ImGuiColorEditFlags_NoLabel |
                                                          ImGuiColorEditFlags_AlphaPreviewHalf;
                        ImGui::SetNextItemWidth( squareSz * 12.0f ); // Use 256 + bar sizes?

                        valueChanged |= colorPicker( "##picker", floatVector, pickerFlags, &g.ColorPickerRef );

                        if( valueChanged ) {
                            outColor = floatVector;
                        }
                    }
                    ImGui::EndPopup();
                }
            }

            ImGui::EndHorizontal();
        } else if( ( flags & ImGuiColorEditFlags_DisplayHex ) != 0 && ( flags & ImGuiColorEditFlags_NoInputs ) == 0 ) {
            //// RGB Hexadecimal Input
            // char buf[64];
            // if (alpha)
            //     ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0,
            //     255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
            // else
            //     ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255),
            //     ImClamp(i[2], 0, 255));
            // ImGui::SetNextItemWidth(w_inputs);
            // if (ImGui::InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsUppercase))
            //{
            //     value_changed = true;
            //     char* p = buf;
            //     while (*p == '#' || ImCharIsBlankA(*p))
            //         p++;
            //     i[0] = i[1] = i[2] = 0;
            //     i[3] = 0xFF; // alpha default to 255 is not parsed by scanf (e.g. inputting #FFFFFF omitting alpha)
            //     int r;
            //     if (alpha)
            //         r = sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned
            //         int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
            //     else
            //         r = sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
            //     IM_UNUSED(r); // Fixes C6031: Return value ignored: 'sscanf'.
            // }
            if( !( flags & ImGuiColorEditFlags_NoOptions ) )
                ImGui::OpenPopupOnItemClick( "context", ImGuiPopupFlags_MouseButtonRight );
        }

        if( id.data() != labelDisplayEnd && !( flags & ImGuiColorEditFlags_NoLabel ) ) {
            // Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition ==
            // ImGuiDir_Left), but we need to use SameLine() to setup baseline correctly. Might want to refactor
            // SameLine() to simplify this.
            ImGui::SameLine( 0.0f, style.ItemInnerSpacing.x );
            window->DC.CursorPos.x = pos.x + ( ( flags & ImGuiColorEditFlags_NoInputs )
                                                   ? wButton
                                                   : wFull + style.ItemInnerSpacing.x );
            ImGui::TextEx( id.data(), labelDisplayEnd );
        }

        // Convert back
        if( valueChanged && pickerActiveWindow == nullptr ) {
            if( !valueChangedAsFloat ) {
                constexpr uint32_t ComponentCount = HasAlpha ? 4 : 3;
                for( uint32_t n = 0; n < ComponentCount; n++ )
                    floatVector[ n ] = intVector[ n ] / 255.0f;
            }

            if( ( flags & ImGuiColorEditFlags_DisplayHSV ) && ( flags & ImGuiColorEditFlags_InputRGB ) ) {
                g.ColorEditSavedHue = floatVector[ 0 ];
                g.ColorEditSavedSat = floatVector[ 1 ];
                ImGui::ColorConvertHSVtoRGB( floatVector[ 0 ],
                                             floatVector[ 1 ],
                                             floatVector[ 2 ],
                                             floatVector[ 0 ],
                                             floatVector[ 1 ],
                                             floatVector[ 2 ] );
                g.ColorEditSavedID = g.ColorEditCurrentID;
                g.ColorEditSavedColor = ImGui::ColorConvertFloat4ToU32(
                    ImVec4( floatVector[ 0 ], floatVector[ 1 ], floatVector[ 2 ], 0 ) );
            }
            if( ( flags & ImGuiColorEditFlags_DisplayRGB ) && ( flags & ImGuiColorEditFlags_InputHSV ) )
                ImGui::ColorConvertRGBtoHSV( floatVector[ 0 ],
                                             floatVector[ 1 ],
                                             floatVector[ 2 ],
                                             floatVector[ 0 ],
                                             floatVector[ 1 ],
                                             floatVector[ 2 ] );

            outColor = floatVector;
        }

        if( setCurrentColorEditId )
            g.ColorEditCurrentID = 0;
        // ImGui::PopID();

        ImGui::EndGroup();
    }

    // Drag and Drop Target
    // NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
    // if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(g.LastItemData.ItemFlags &
    // ImGuiItemFlags_ReadOnly) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
    //{
    //    bool accepted_drag_drop = false;
    //    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
    //    {
    //        memcpy((float*)outColor.X, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
    //        value_changed = accepted_drag_drop = true;
    //    }
    //    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
    //    {
    //        memcpy((float*)outColor.X, payload->Data, sizeof(float) * components);
    //        value_changed = accepted_drag_drop = true;
    //    }

    //    // Drag-drop payloads are always RGB
    //    if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
    //        ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
    //    ImGui::EndDragDropTarget();
    //}

    //// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
    // if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
    //     g.LastItemData.ID = g.ActiveId;

    if( valueChanged &&
        g.LastItemData.ID != 0 ) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
        ImGui::MarkItemEdited( g.LastItemData.ID );

    return valueChanged;
}
} // namespace
bool ColorInput( StringView id, Vector3u8& rgb ) {
    Vector3f32 rgba( static_cast< float32 >( rgb.X ),
                     static_cast< float32 >( rgb.Y ),
                     static_cast< float32 >( rgb.Z ) );
    if( colorEdit( id, rgba, ImGuiColorEditFlags_NoAlpha ) ) {
        rgb.X = static_cast< uint8_t >( rgba.X );
        rgb.Y = static_cast< uint8_t >( rgba.Y );
        rgb.Z = static_cast< uint8_t >( rgba.Z );
        return true;
    }

    return false;
}

bool ColorInput( StringView id, Vector4u8& rgba ) {
    ONYX_UNUSED( id );
    ONYX_UNUSED( rgba );

    return true;
}

bool ColorInput( StringView id, Vector3f32& hsv ) {
    Vector3f32 rgb( hsv );
    if( colorEdit( id, rgb, ImGuiColorEditFlags_None ) ) {
        hsv = rgb;
        return true;
    }

    return false;
}

bool ColorInput( StringView id, Vector4f32& hsva ) {
    ONYX_UNUSED( id );
    ONYX_UNUSED( hsva );
    return true;
}
} // namespace onyx::ui
