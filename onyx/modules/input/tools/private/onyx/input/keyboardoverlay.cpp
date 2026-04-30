#include <onyx/input/keyboardoverlay.h>

#if ONYX_USE_IMGUI

#include <onyx/filesystem/onyxfile.h>
#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/theme/theme.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::input::tools {
namespace {
void drawLabel( ImDrawList* dl, ImVec2 tl, ImVec2 br, const char* label, ImU32 color ) {
    if( !label || label[ 0 ] == '\0' )
        return;

    ImFont* font = ImGui::GetFont();
    float keyW = br.x - tl.x;
    float keyH = br.y - tl.y;

    const float nativeSz = font->FontSize; // e.g. 16
    const float targetH = std::min( keyH, keyW ) * 0.60f;
    const float desiredSz = std::min( targetH, nativeSz ); // never upscale
    const float savedScale = font->Scale;
    font->Scale = desiredSz / nativeSz;

    ImVec2 textSz = font->CalcTextSizeA( font->FontSize * font->Scale, FLT_MAX, 0.f, label );

    if( textSz.x <= keyW - 2.0f && textSz.y <= keyH - 2.0f ) {
        ImVec2 textPos = { tl.x + ( keyW - textSz.x ) * 0.5f, tl.y + ( keyH - textSz.y ) * 0.5f };

        dl->PushClipRect( tl, br, true );
        dl->AddText( font, 0.f, textPos, color, label );
        dl->PopClipRect();
    }

    font->Scale = savedScale; // always restore
}

void drawLShapeKey( ImDrawList* dl,
                    ImVec2 tl1,
                    ImVec2 br1, // primary rect
                    ImVec2 tl2,
                    ImVec2 br2, // secondary rect
                    bool pressed,
                    StringView label ) {
    const ImU32 fill = pressed ? ImGui::GetColorU32( ImGuiCol_ButtonActive ) : ImGui::GetColorU32( ImGuiCol_Button );
    const ImU32 outline = pressed ? ImGui::GetColorU32( ImGuiCol_Border ) : ImGui::GetColorU32( ImGuiCol_BorderShadow );
    const ImU32 lblCol = ImGui::GetColorU32( ImGuiCol_Text );

    const auto& style = ImGui::GetStyle();
    const float32 frameBorderSize = style.FrameBorderSize;
    const float32 frameRounding = style.FrameRounding;

    // Determine which rect is the "top narrow" part and which is the
    // "bottom wider" part (ISO Enter: top is narrow-right, bottom is wide-left).
    // We identify this by comparing left edges: the secondary rect usually
    // extends further left (smaller x).
    //
    // Build the 6-point clockwise L-polygon:
    //
    //   A ──────── B
    //   |          |
    //   C ──── D   |   (if secondary is wider to the left)
    //          |   |
    //          E ── F
    //
    // Where the exact corners depend on which side the step is on.

    // Normalise: rect1 = top (smaller y), rect2 = bottom (larger y)
    ImVec2 r1tl = tl1, r1br = br1;
    ImVec2 r2tl = tl2, r2br = br2;
    if( r2tl.y < r1tl.y ) {
        std::swap( r1tl, r2tl );
        std::swap( r1br, r2br );
    }

    // The 6 vertices of the L (clockwise):
    //  Top-left, Top-right, step-right / step-left inner corner,
    //  bottom-right, bottom-left, step inner corner on the other side.
    //
    // For ISO Enter the secondary (bottom) rect is wider to the LEFT,
    // so the step is on the left side.
    ImVec2 poly[ 7 ];

    if( r2tl.x < r1tl.x ) // step on left  (ISO Enter)
    {
        poly[ 0 ] = { r2tl.x, r2tl.y }; // A
        poly[ 1 ] = { r1br.x, r2tl.y }; // B
        poly[ 2 ] = { r1br.x, r1br.y }; // C
        poly[ 3 ] = { r1tl.x, r1br.y }; // D
        poly[ 4 ] = { r1tl.x, r2br.y }; // E  ← inner step
        poly[ 5 ] = { r2tl.x, r2br.y }; // F  ← inner step
    } else                              // step on right (numpad + / big Enter variants)
    {
        poly[ 0 ] = { r1tl.x, r1tl.y }; // A
        poly[ 1 ] = { r1br.x, r1tl.y }; // B
        poly[ 2 ] = { r1br.x, r2br.y }; // C  ← inner step
        poly[ 3 ] = { r1br.x, r2br.y }; // D  ← inner step
        poly[ 4 ] = { r1br.x, r2tl.y }; // E
        poly[ 5 ] = { r2tl.x, r2br.y }; // F
    }

    // Filled polygon (no rounding — we'll soften corners with a nudge below)
    dl->AddConcavePolyFilled( poly, 6, fill );

    // Bevel highlight on top edge only
    if( !pressed ) {
        ImU32 bevel = IM_COL32( 255, 255, 255, 18 );
        dl->AddLine( { poly[ 0 ].x + frameRounding, poly[ 0 ].y + 0.5f },
                     { poly[ 1 ].x - frameRounding, poly[ 1 ].y + 0.5f },
                     bevel,
                     1.0f );
    }

    dl->AddPolyline( poly, 6, outline, ImDrawFlags_Closed, frameBorderSize );

    if( label.empty() == false ) {
        // Label centred in the bounding box of the whole shape
        ImVec2 bbTL = { std::min( r1tl.x, r2tl.x ), r1tl.y };
        ImVec2 bbBR = { std::max( r1br.x, r2br.x ), r2br.y };
        drawLabel( dl, bbTL, bbBR, label.data(), lblCol );
    }
}

void drawRectKey( ImDrawList* dl, ImVec2 tl, ImVec2 br, bool pressed, StringView label ) {
    const ImU32 fill = pressed ? ImGui::GetColorU32( ImGuiCol_ButtonActive ) : ImGui::GetColorU32( ImGuiCol_Button );
    const ImU32 outline = pressed ? ImGui::GetColorU32( ImGuiCol_Border ) : ImGui::GetColorU32( ImGuiCol_BorderShadow );
    const ImU32 lblCol = ImGui::GetColorU32( ImGuiCol_Text );

    const auto& style = ImGui::GetStyle();
    const float32 frameBorderSize = style.FrameBorderSize;
    const float32 frameRounding = style.FrameRounding;
    dl->AddRectFilled( tl, br, fill, frameRounding );

    if( !pressed ) {
        ImU32 bevel = IM_COL32( 255, 255, 255, 18 );
        dl->AddLine( { tl.x + frameRounding, tl.y + 0.5f }, { br.x - frameRounding, tl.y + 0.5f }, bevel, 1.0f );
    }

    dl->AddRect( tl, br, outline, frameRounding, 0, frameBorderSize );

    if( label.empty() == false ) {
        drawLabel( dl, tl, br, label.data(), lblCol );
    }
}

} // namespace
KeyboardOverlay::KeyboardOverlay() {
    file_system::OnyxFile file{ StringView( "engine:/debug/keyboard/iso_105_us_international.json" ) };
    file_system::JsonValue jsonDoc = file.LoadJson();

    m_keyboardLayout.reserve( 6 ); // typical keyboards have 6 rows

    Vector2f32 offset;
    Vector2f32 offset2; // optional
    Vector2f32 size{ 1.0f, 1.0f };
    Vector2f32 size2{ 0.0f, 0.0f }; // optional

    for( const auto& rowData : jsonDoc.Json ) {
        DynamicArray< KeyData >& keyboardRow = m_keyboardLayout.emplace_back();
        for( const auto& columnValue : rowData ) {
            if( columnValue.is_object() == false ) {
                ONYX_LOG_WARNING( "Skipping element in keyboard json" );
                continue;
            }

            // is key data
            if( columnValue.contains( "label" ) ) {
                const String& label = columnValue[ "label" ].get< String >();
                input::Key key = enums::toEnum< input::Key >( columnValue[ "keyCode" ].get< uint16_t >() );
                keyboardRow.emplace_back( offset, offset2, size, size2, key, label );

                offset.set( 0.0f, 0.0f );
                offset2.set( 0.0f, 0.0f );
                size.set( 1.0f, 1.0f );
                size2.set( 0.0f, 0.0f );
            } else // is meta data
            {
                if( columnValue.contains( "x" ) )
                    offset[ 0 ] = columnValue[ "x" ].get< float >();
                if( columnValue.contains( "x2" ) )
                    offset2[ 0 ] = columnValue[ "x2" ].get< float >();

                if( columnValue.contains( "y" ) )
                    offset[ 1 ] = columnValue[ "y" ].get< float >();
                if( columnValue.contains( "y2" ) )
                    offset2[ 1 ] = columnValue[ "y2" ].get< float >();

                if( columnValue.contains( "w" ) )
                    size[ 0 ] = columnValue[ "w" ].get< float >();
                if( columnValue.contains( "w2" ) )
                    size2[ 0 ] = columnValue[ "w2" ].get< float >();

                if( columnValue.contains( "h" ) )
                    size[ 1 ] = columnValue[ "h" ].get< float >();
                if( columnValue.contains( "h2" ) )
                    size2[ 1 ] = columnValue[ "h2" ].get< float >();
            }
        }
    }
}

void KeyboardOverlay::onOpen() {
    setDefaultPosition( ui::WindowPosition::BottomLeft );

    setWindowFlags( ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking );
    setDefaultSize( { 400, -1 } );

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

void KeyboardOverlay::onRender( ui::ImGuiSystem& imguiSystem ) {
    const input::InputSystem& inputSystem = *ui::g_uiContext.InputSystem;

    ImGui::BringWindowToDisplayFront( ImGui::GetCurrentWindow() );

    static constexpr float KLayoutWidthUnits = 23.0f;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const float unitSize = availableWidth / KLayoutWidthUnits;
    const ImVec2 origin = ImGui::GetCursorScreenPos();

    float totalHeight = 0.0f;
    ImVec2 cursorPos = origin;

    const ImVec2 itemSpacing = { 2, 2 };

    bool showLabels = false;
    for( const DynamicArray< KeyData >& keyboardRow : m_keyboardLayout ) {
        for( const KeyData& key : keyboardRow ) {
            const bool pressed = inputSystem.IsButtonDown( key.Key );

            ImVec2 tl = { cursorPos.x + key.Offset[ 0 ] * unitSize, cursorPos.y + key.Offset[ 1 ] * unitSize };
            ImVec2 br = { tl.x + key.Size[ 0 ] * unitSize - itemSpacing.x,
                          tl.y + key.Size[ 1 ] * unitSize - itemSpacing.y };

            if( key.Size2.isZero() ) {
                drawRectKey( dl, tl, br, pressed, showLabels ? key.Label : "" );
            } else {
                ImVec2 tl2 = { tl.x + key.Offset2[ 0 ] * unitSize, tl.y + key.Offset2[ 1 ] * unitSize };
                ImVec2 br2 = { tl2.x + key.Size2[ 0 ] * unitSize - itemSpacing.x,
                               tl2.y + key.Size2[ 1 ] * unitSize - itemSpacing.y };

                drawLShapeKey( dl, tl, br, tl2, br2, pressed, showLabels ? key.Label : "" );
            }

            cursorPos.x += ( key.Offset[ 0 ] + key.Size[ 0 ] ) * unitSize;
            cursorPos.y += key.Offset[ 1 ] * unitSize;

            totalHeight = std::max( totalHeight, br.y - origin.y );
        }

        cursorPos.x = origin.x;
        cursorPos.y += unitSize;
        totalHeight = cursorPos.y - origin.y;
    }

    // Reserve space → window auto-sizes correctly
    ImGui::Dummy( ImVec2( availableWidth, totalHeight ) );
}
} // namespace onyx::input::tools
#endif
