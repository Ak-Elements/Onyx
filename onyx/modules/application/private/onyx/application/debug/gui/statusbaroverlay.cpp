#include <onyx/application/debug/gui/statusbaroverlay.h>

#if ONYX_UI_MODULE
#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/controls/button.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace onyx::application::debug {
void StatusBarOverlay::OnRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    ::ImGuiWindow* window = ImGui::FindWindowByName( "SceneView" );
    if ( window != nullptr ) {
        ImGui::SetNextWindowPos( window->InnerRect.Min + ImGui::GetStyle().FramePadding );
        ImGui::SetNextWindowSize( window->Size );
    } else {
        return;
    }

    ImGui::Begin( "##debugOverlay",
                  nullptr,
                  ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav );

    ImGui::BeginHorizontal( "items" );

    bool hasOverlayClosed = false;
    for ( UniquePtr< StatusBarOverlayItem >& overlay : m_items ) {
        const StringView overlayName = overlay->getName();
        const StringView value = overlay->getValue();

        bool hasClicked = ui::Button( value.data(), Vector2f32( 100, 0 ) );

        if ( hasClicked ) {
            bool open = true;
            ImGui::Begin( overlayName.data(), &open );
            ImGui::End();
        }
    }

    ImGui::EndHorizontal();

    ImGui::End();

    if ( hasOverlayClosed ) {
        std::erase_if( m_items, []( const UniquePtr< StatusBarOverlayItem >& overlay ) { return overlay == nullptr; } );
    }
}
} // namespace onyx::application::debug

#endif