#include <onyx/ui/windows/statusbaroverlay.h>

#include <onyx/ui/controls/button.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace onyx::ui {
void StatusBarOverlay::onOpen() {
    setWindowFlags( ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoNav );

    setDefaultPosition( WindowPosition::TopLeft );
}

void StatusBarOverlay::onRender( ImGuiSystem& /*imguiSystem*/ ) {
    ImGui::BeginHorizontal( "items" );

    bool hasOverlayClosed = false;
    for( UniquePtr< StatusBarOverlayItem >& overlay : m_items ) {
        const StringView overlayName = overlay->getName();
        const StringView value = overlay->getValue();

        bool hasClicked = ui::Button( value.data(), Vector2f32( 100, 0 ) );

        if( hasClicked ) {
            bool open = true;
            ImGui::Begin( overlayName.data(), &open );
            ImGui::End();
        }
    }

    ImGui::EndHorizontal();

    if( hasOverlayClosed ) {
        std::erase_if( m_items, []( const UniquePtr< StatusBarOverlayItem >& overlay ) { return overlay == nullptr; } );
    }
}
} // namespace onyx::ui
