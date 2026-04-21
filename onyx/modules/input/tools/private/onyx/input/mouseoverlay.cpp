#include <onyx/input/mouseoverlay.h>

#if ONYX_USE_IMGUI

#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::input::tools {
namespace {
void addRectFilledMultiColor( ImDrawList* drawList,
                              const ImVec2& pMin,
                              const ImVec2& pMax,
                              ImU32 colUprLeft,
                              ImU32 colUprRight,
                              ImU32 colBotRight,
                              ImU32 colBotLeft,
                              float rounding,
                              ImDrawFlags flags ) {
    if( ( ( colUprLeft | colUprRight | colBotRight | colBotLeft ) & IM_COL32_A_MASK ) == 0 )
        return;

    if( ( flags & ImDrawFlags_RoundCornersMask_ ) == 0 )
        flags |= ImDrawFlags_RoundCornersAll;

    rounding = ImMin( rounding,
                      ImFabs( pMax.x - pMin.x ) *
                              ( ( ( flags & ImDrawFlags_RoundCornersTop ) == ImDrawFlags_RoundCornersTop ) ||
                                        ( ( flags & ImDrawFlags_RoundCornersBottom ) == ImDrawFlags_RoundCornersBottom )
                                    ? 0.5f
                                    : 1.0f ) -
                          1.0f );
    rounding = ImMin( rounding,
                      ImFabs( pMax.y - pMin.y ) *
                              ( ( ( flags & ImDrawFlags_RoundCornersLeft ) == ImDrawFlags_RoundCornersLeft ) ||
                                        ( ( flags & ImDrawFlags_RoundCornersRight ) == ImDrawFlags_RoundCornersRight )
                                    ? 0.5f
                                    : 1.0f ) -
                          1.0f );

    // https://github.com/ocornut/imgui/issues/3710#issuecomment-758555966
    if( rounding > 0.0f ) {
        const int sizeBefore = drawList->VtxBuffer.Size;
        drawList->AddRectFilled( pMin, pMax, IM_COL32_WHITE, rounding, flags );
        const int sizeAfter = drawList->VtxBuffer.Size;

        for( int i = sizeBefore; i < sizeAfter; i++ ) {
            ImDrawVert* vert = drawList->VtxBuffer.Data + i;

            ImVec4 uprLeft = ImGui::ColorConvertU32ToFloat4( colUprLeft );
            ImVec4 botLeft = ImGui::ColorConvertU32ToFloat4( colBotLeft );
            ImVec4 upRight = ImGui::ColorConvertU32ToFloat4( colUprRight );
            ImVec4 botRight = ImGui::ColorConvertU32ToFloat4( colBotRight );

            float x = ImClamp( ( vert->pos.x - pMin.x ) / ( pMax.x - pMin.x ), 0.0f, 1.0f );

            // 4 colors - 8 deltas

            float r1 = uprLeft.x + ( upRight.x - uprLeft.x ) * x;
            float r2 = botLeft.x + ( botRight.x - botLeft.x ) * x;

            float g1 = uprLeft.y + ( upRight.y - uprLeft.y ) * x;
            float g2 = botLeft.y + ( botRight.y - botLeft.y ) * x;

            float b1 = uprLeft.z + ( upRight.z - uprLeft.z ) * x;
            float b2 = botLeft.z + ( botRight.z - botLeft.z ) * x;

            float a1 = uprLeft.w + ( upRight.w - uprLeft.w ) * x;
            float a2 = botLeft.w + ( botRight.w - botLeft.w ) * x;

            float y = ImClamp( ( vert->pos.y - pMin.y ) / ( pMax.y - pMin.y ), 0.0f, 1.0f );
            float r = r1 + ( r2 - r1 ) * y;
            float g = g1 + ( g2 - g1 ) * y;
            float b = b1 + ( b2 - b1 ) * y;
            float a = a1 + ( a2 - a1 ) * y;
            ImVec4 rgba( r, g, b, a );

            rgba = rgba * ImGui::ColorConvertU32ToFloat4( vert->col );

            vert->col = ImColor( rgba );
        }
        return;
    }

    const ImVec2 uv = drawList->_Data->TexUvWhitePixel;
    drawList->PrimReserve( 6, 4 );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx ) );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx + 1 ) );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx + 2 ) );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx ) );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx + 2 ) );
    drawList->PrimWriteIdx( (ImDrawIdx)( drawList->_VtxCurrentIdx + 3 ) );
    drawList->PrimWriteVtx( pMin, uv, colUprLeft );
    drawList->PrimWriteVtx( ImVec2( pMax.x, pMin.y ), uv, colUprRight );
    drawList->PrimWriteVtx( pMax, uv, colBotRight );
    drawList->PrimWriteVtx( ImVec2( pMin.x, pMax.y ), uv, colBotLeft );
}
void drawScrollWheel( ImDrawList* dl,
                      ImVec2 wheelTopLeft,
                      float32 width,
                      float32 height,
                      float32 intensity,
                      bool middleDown,
                      const MouseOverlay::Style& /*style*/,
                      const MouseOverlay::Colors& colors ) {
    const float32 scale = 1.0f;

    const bool isScrollUp = intensity > 0.0f;
    const bool isScrollDown = intensity < 0.0f;

    ImVec4 baseColor = middleDown ? colors.WheelPressed : colors.WheelIdle;

    // Choose which scroll colour is the "lit" tip
    ImVec4 tipColor = isScrollUp ? colors.WheelPressedScrollUp : colors.WheelPressedScrollDown;

    auto lerp4 = []( const ImVec4& a, const ImVec4& b, float32 t ) -> ImVec4 {
        return { a.x + ( b.x - a.x ) * t, a.y + ( b.y - a.y ) * t, a.z + ( b.z - a.z ) * t, a.w + ( b.w - a.w ) * t };
    };

    ImVec4 litEnd = lerp4( baseColor, tipColor, std::abs( intensity ) );
    ImVec4 darkEnd = baseColor;

    // Which physical end of the wheel gets the lit colour
    ImU32 topColor = ImGui::ColorConvertFloat4ToU32( isScrollUp ? litEnd : darkEnd );
    ImU32 bottomColor = ImGui::ColorConvertFloat4ToU32( isScrollDown ? litEnd : darkEnd );
    ImVec2 wheelBottomRight = { wheelTopLeft.x + width, wheelTopLeft.y + height };

    addRectFilledMultiColor( dl,
                             wheelTopLeft,
                             wheelBottomRight,
                             topColor,
                             topColor,
                             bottomColor,
                             bottomColor,
                             3.0f,
                             ImDrawFlags_RoundCornersAll );

    dl->AddRect( wheelTopLeft,
                 wheelBottomRight,
                 ImGui::ColorConvertFloat4ToU32( colors.BodyOutline ),
                 3.0f,
                 ImDrawFlags_RoundCornersAll,
                 1.4f * scale );

    ImU32 wheelIndentColor = IM_COL32( 0, 0, 0, (int)( 70 * ( 1.f / 0.75f ) ) );

    for( int32_t i = 1; i <= 3; ++i ) {
        float32 lineY = wheelTopLeft.y + height * ( static_cast< float32 >( i ) / 4.f );
        dl->AddLine( { wheelTopLeft.x + 2 * scale, lineY },
                     { wheelTopLeft.x + width - 2 * scale, lineY },
                     wheelIndentColor,
                     1.0f * scale );
    }
}

} // namespace
void MouseOverlay::onOpen() {
    setDefaultPosition( ui::WindowPosition::BottomLeft );

    setWindowFlags( ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking );
    setDefaultSize( { 118, -1 } );

    // float32 aspectRatio = 23.0f / static_cast< float32 >( m_keyboardLayout.size() + 1 );
    // auto aspect_ratio_constraint = []( ImGuiSizeCallbackData* data ) {
    //     float aspectRatio = *static_cast< float32* >( data->UserData );
    //     data->DesiredSize.y = (float)(int)( data->DesiredSize.x / aspectRatio );
    // };
    //
    // ImGui::SetNextWindowSize( ImVec2( 400, 600 ), ImGuiCond_FirstUseEver );
    // ImGui::SetNextWindowSizeConstraints( ImVec2( 200, 300 ),
    //                                      ImVec2( FLT_MAX, FLT_MAX ),
    //                                      aspect_ratio_constraint,
    //                                      &aspectRatio ); // Aspect ratio
}

void MouseOverlay::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    const input::InputSystem& inputSystem = *ui::g_UiContext.InputSystem;

    ImGui::BringWindowToDisplayFront( ImGui::GetCurrentWindow() );

    const float32 scale = 1.0f; // cfg.ScaleMultiplier;
    const float32 width = s_style.Width * scale;
    const float32 height = s_style.Height * scale;
    const float32 bodyRounding = s_style.BodyRounding * scale;
    const float32 buttonRounding = s_style.ButtonRounding * scale;
    const float32 buttonHeight = s_style.ButtonHeight * scale;

    const float32 wheelWidth = s_style.WheelWidth * scale;
    const float32 wheelHeight = s_style.WheelHeight * scale;

    const bool leftDown = inputSystem.IsButtonDown( MouseButton::Button_1 );
    const bool rightDown = inputSystem.IsButtonDown( MouseButton::Button_2 );
    const bool isMiddleDown = inputSystem.IsButtonDown( MouseButton::Button_3 );

    ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Body
    dl->AddRectFilled( { cursorPosition.x, cursorPosition.y },
                       { cursorPosition.x + width, cursorPosition.y + height },
                       ImGui::ColorConvertFloat4ToU32( s_colors.BodyBg ),
                       bodyRounding );

    dl->AddRect( { cursorPosition.x, cursorPosition.y },
                 { cursorPosition.x + width, cursorPosition.y + height },
                 ImGui::ColorConvertFloat4ToU32( s_colors.BodyOutline ),
                 bodyRounding,
                 0,
                 1.8f * scale );

    // Left button
    ImVec4 lCol = leftDown ? s_colors.LeftPressed : s_colors.ButtonIdle;
    dl->AddRectFilled( { cursorPosition.x, cursorPosition.y },
                       { cursorPosition.x + width / 2.f, cursorPosition.y + buttonHeight },
                       ImGui::ColorConvertFloat4ToU32( lCol ),
                       buttonRounding,
                       ImDrawFlags_RoundCornersTopLeft );

    dl->AddRect( { cursorPosition.x, cursorPosition.y },
                 { cursorPosition.x + width / 2.f, cursorPosition.y + buttonHeight },
                 ImGui::ColorConvertFloat4ToU32( s_colors.BodyOutline ),
                 buttonRounding,
                 ImDrawFlags_RoundCornersTopLeft,
                 1.2f * scale );

    // Right button
    ImVec4 rCol = rightDown ? s_colors.RightPressed : s_colors.ButtonIdle;
    dl->AddRectFilled( { cursorPosition.x + width / 2.f, cursorPosition.y },
                       { cursorPosition.x + width, cursorPosition.y + buttonHeight },
                       ImGui::ColorConvertFloat4ToU32( rCol ),
                       buttonRounding,
                       ImDrawFlags_RoundCornersTopRight );

    dl->AddRect( { cursorPosition.x + width / 2.f, cursorPosition.y },
                 { cursorPosition.x + width, cursorPosition.y + buttonHeight },
                 ImGui::ColorConvertFloat4ToU32( s_colors.BodyOutline ),
                 buttonRounding,
                 ImDrawFlags_RoundCornersTopRight,
                 1.2f * scale );

    // Centre divider
    dl->AddLine( { cursorPosition.x + width / 2.f, cursorPosition.y },
                 { cursorPosition.x + width / 2.f, cursorPosition.y + buttonHeight },
                 ImGui::ColorConvertFloat4ToU32( s_colors.Divider ),
                 1.5f * scale );

    // Scroll wheel
    float32 whlX = cursorPosition.x + width / 2.f - wheelWidth / 2.f;
    float32 whlY = cursorPosition.y + buttonHeight / 2.f - wheelHeight / 2.f;

    const float dt = ImGui::GetIO().DeltaTime;
    const float decayTime = 0.2f; // seconds
    const float decaySpeed = dt / decayTime;

    int32_t wheelDelta = inputSystem.GetMouseWheelDelta();
    if( wheelDelta != 0 ) {
        // Snap intensity 60 % of the way toward the target direction each event
        float target = wheelDelta > 0 ? 1.f : -1.f;
        m_scrollIntensity += ( target - m_scrollIntensity ) * 0.60f;
        m_scrollIntensity = ImClamp( m_scrollIntensity, -1.f, 1.f );
    } else if( m_scrollIntensity != 0.0f ) {
        m_scrollIntensity *= 1.0f - decaySpeed;
        if( isEqual( m_scrollIntensity, 0.001f ) )
            m_scrollIntensity = 0.0f;
    }

    drawScrollWheel( dl, { whlX, whlY }, wheelWidth, wheelHeight, m_scrollIntensity, isMiddleDown, s_style, s_colors );

    ImGui::Dummy( { width, height } );
}
} // namespace onyx::input::tools
#endif
