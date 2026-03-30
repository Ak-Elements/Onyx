#include <onyx/input/keyboardoverlay.h>

#if ONYX_USE_IMGUI

#include <onyx/filesystem/onyxfile.h>
#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::input::tools {

KeyboardOverlay::KeyboardOverlay() {
    file_system::OnyxFile file{ StringView( "engine:/debug/keyboard/iso_105_us_international.json" ) };
    file_system::JsonValue jsonDoc = file.LoadJson();

    m_keyboardLayout.reserve( 6 ); // typical keyboards have 6 rows

    Vector2f32 offset;
    Vector2f32 offset2; // optional
    Vector2f32 size{ 1.0f, 1.0f };
    Vector2f32 size2{ 0.0f, 0.0f }; // optional

    for ( const auto& rowData : jsonDoc.Json ) {
        DynamicArray< KeyData >& keyboardRow = m_keyboardLayout.emplace_back();
        for ( const auto& columnValue : rowData ) {
            if ( columnValue.is_object() == false ) {
                ONYX_LOG_WARNING( "Skipping element in keyboard json" );
                continue;
            }

            // is key data
            if ( columnValue.contains( "label" ) ) {
                const String& label = columnValue[ "label" ].get< String >();
                input::Key key = enums::toEnum< input::Key >( columnValue[ "keyCode" ].get< uint16_t >() );
                keyboardRow.emplace_back( offset, offset2, size, size2, key, label );

                offset.set( 0.0f, 0.0f );
                offset2.set( 0.0f, 0.0f );
                size.set( 1.0f, 1.0f );
                size2.set( 0.0f, 0.0f );
            } else // is meta data
            {
                if ( columnValue.contains( "x" ) )
                    offset[ 0 ] = columnValue[ "x" ].get< float >();
                if ( columnValue.contains( "x2" ) )
                    offset2[ 0 ] = columnValue[ "x2" ].get< float >();

                if ( columnValue.contains( "y" ) )
                    offset[ 1 ] = columnValue[ "y" ].get< float >();
                if ( columnValue.contains( "y2" ) )
                    offset2[ 1 ] = columnValue[ "y2" ].get< float >();

                if ( columnValue.contains( "w" ) )
                    size[ 0 ] = columnValue[ "w" ].get< float >();
                if ( columnValue.contains( "w2" ) )
                    size2[ 0 ] = columnValue[ "w2" ].get< float >();

                if ( columnValue.contains( "h" ) )
                    size[ 1 ] = columnValue[ "h" ].get< float >();
                if ( columnValue.contains( "h2" ) )
                    size2[ 1 ] = columnValue[ "h2" ].get< float >();
            }
        }
    }
}

void KeyboardOverlay::onOpen() {}

void KeyboardOverlay::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    if ( ui::g_UiContext.InputSystem == nullptr )
        return;

    const input::InputSystem& inputSystem = *ui::g_UiContext.InputSystem;
    // setWindowFlags( ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking );
    //
    // // TODO: This should probably be rewritten to use ItemAdd from Imgui to auto-size the window
    // setDefaultPosition( ui::WindowPosition::BottomLeft );
    //
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
    // begin();
    ImGui::BringWindowToDisplayFront( ImGui::GetCurrentWindow() );

    static const ImVec4 defaultColor{ 0.7f, 0.7f, 0.7f, 1.0f };
    static const ImVec4 pressedColor{ 0.9f, 0.2f, 0.2f, 1.0f };
    uint32_t defaultColorU32 = ImGui::GetColorU32( defaultColor );
    uint32_t pressedColorU32 = ImGui::GetColorU32( pressedColor );
    auto drawList = ImGui::GetWindowDrawList();
    const float32 renderSize = ImGui::GetContentRegionAvail().x;
    float size = renderSize / 23.0f;
    const ImVec2& initialPosition = ImGui::GetCursorScreenPos();
    Vector2f32 cursorPos = { initialPosition.x, initialPosition.y };
    float spacing = 1.0f;

    ImRect keyBounds;
    for ( const DynamicArray< KeyData >& keyboardRow : m_keyboardLayout ) {
        uint32_t numButtonsInRow = static_cast< uint32_t >( keyboardRow.size() );
        for ( uint32_t i = 0; i < numButtonsInRow; ++i ) {
            const KeyData& key = keyboardRow[ i ];
            const Vector2f32& rectSize = key.Size * size;

            Vector2f32 tempCursorPos = cursorPos;

            uint32_t color = inputSystem.IsButtonDown( key.Key ) ? pressedColorU32 : defaultColorU32;

            tempCursorPos += ( key.Offset * size );
            keyBounds = ImRect(
                { tempCursorPos[ 0 ], tempCursorPos[ 1 ] },
                { tempCursorPos[ 0 ] + rectSize[ 0 ] - spacing, tempCursorPos[ 1 ] + rectSize[ 1 ] - spacing } );
            drawList->AddRectFilled( keyBounds.Min, keyBounds.Max, color );

            if ( key.Size2.isZero() == false ) {
                const Vector2f32& rectSize2 = key.Size2 * size;
                Vector2f32 tmpCursorPos2 = tempCursorPos + ( key.Offset2 * size );
                keyBounds = ImRect(
                    { tmpCursorPos2[ 0 ], tmpCursorPos2[ 1 ] },
                    { tmpCursorPos2[ 0 ] + rectSize2[ 0 ] - spacing, tmpCursorPos2[ 1 ] + rectSize2[ 1 ] - spacing } );
                drawList->AddRectFilled( keyBounds.Min, keyBounds.Max, color );
            }

            cursorPos = cursorPos + ( key.Offset * size );
            cursorPos[ 0 ] += rectSize[ 0 ];
        }

        cursorPos[ 0 ] = initialPosition.x;
        cursorPos[ 1 ] += size;
    }

    // ImVec2 windowSize = ImVec2(initialPosition.x + renderSize, cursorPos[1]) - initialPosition;
    // ImGui::Dummy(windowSize);
    // end();
}

} // namespace onyx::input::tools
#endif
