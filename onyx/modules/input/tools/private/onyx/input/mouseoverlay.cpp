#include <onyx/input/mouseoverlay.h>

#if ONYX_USE_IMGUI

#include <onyx/input/inputsystem.h>
#include <onyx/ui/imguisystem.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::input::tools {
namespace {

	void drawScrollWheel(ImDrawList* dl,
                                 ImVec2      wTL,
                                 float whlW, float whlH, float /*whlR*/,
                                 float intensity,
                                 bool  middleDown,
                                 const MouseOverlaySettings& cfg )
    {
        const float S = 1.0f;
		float absI = std::abs(intensity);
 
        ImVec4 base    = middleDown ? cfg.ColorWheelPressed : cfg.ColorWheelIdle;
           
        ImVec2 wBR  = { wTL.x + whlW, wTL.y + whlH };
        ImVec2 wMid = { wTL.x,         wTL.y + whlH * 0.5f };
 
        // Top half gradient quad
        dl->AddRectFilled(
            wTL,  wBR,
             ImGui::ColorConvertFloat4ToU32(base), 3.0f
        );
        
		ImU32 wTexCol = IM_COL32(0, 0, 0, (int)(70 * (1.f - absI / 0.75f)));

		for (int i = 1; i <= 3; ++i)
		{
			float lineY = wTL.y + whlH * (i / 4.f);
			dl->AddLine({ wTL.x + 2*S, lineY },
						{ wTL.x + whlW - 2*S, lineY },
						wTexCol, 1.0f * S);
		}
    }

}
void MouseOverlay::onOpen() {
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

void MouseOverlay::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    
    const input::InputSystem& inputSystem = *ui::g_UiContext.InputSystem;

    static MouseOverlaySettings cfg;

    ImGui::BringWindowToDisplayFront( ImGui::GetCurrentWindow() );
 
	const float S    = 1.0f; //cfg.ScaleMultiplier;
    const float W    = 100.f * S;
    const float H    = 160.f * S;
    const float R    = 50.f  * S;
    const float Rb   = 36.f  * S;
    const float btnH = 65.f  * S;
 
    const float whlW = 14.f * S;
    const float whlH = 28.f * S;
    const float whlR =  7.f * S;
 
    const bool leftDown = inputSystem.IsButtonDown( MouseButton::Button_1 );
    const bool rightDown = inputSystem.IsButtonDown( MouseButton::Button_2 );
    const bool isMiddleDown = inputSystem.IsButtonDown( MouseButton::Button_3 );

    ImVec2 p  = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();
 
    // ── Outer body ────────────────────────────────────────────────────────
    dl->AddRectFilled({ p.x, p.y }, { p.x+W, p.y+H },  ImGui::ColorConvertFloat4ToU32(cfg.ColorBodyBg), R);
    dl->AddRect      ({ p.x, p.y }, { p.x+W, p.y+H },  ImGui::ColorConvertFloat4ToU32(cfg.ColorBodyOutline), R, 0, 1.8f*S);
 
    // ── Left button ───────────────────────────────────────────────────────
    ImVec4 lCol = leftDown ? cfg.ColorLeftPressed : cfg.ColorButtonIdle;
    dl->AddRectFilled({ p.x,       p.y }, { p.x+W/2.f, p.y+btnH },
                      ImGui::ColorConvertFloat4ToU32(lCol), Rb, ImDrawFlags_RoundCornersTopLeft);
    dl->AddRect      ({ p.x,       p.y }, { p.x+W/2.f, p.y+btnH },
                      ImGui::ColorConvertFloat4ToU32(cfg.ColorBodyOutline), Rb, ImDrawFlags_RoundCornersTopLeft, 1.2f*S);
 
    // ── Right button ──────────────────────────────────────────────────────
    ImVec4 rCol = rightDown ? cfg.ColorRightPressed : cfg.ColorButtonIdle;
    dl->AddRectFilled({ p.x+W/2.f, p.y }, { p.x+W, p.y+btnH },
                      ImGui::ColorConvertFloat4ToU32(rCol), Rb, ImDrawFlags_RoundCornersTopRight);
    dl->AddRect      ({ p.x+W/2.f, p.y }, { p.x+W, p.y+btnH },
                      ImGui::ColorConvertFloat4ToU32(cfg.ColorBodyOutline), Rb, ImDrawFlags_RoundCornersTopRight, 1.2f*S);
 
    // ── Centre divider ────────────────────────────────────────────────────
    dl->AddLine({ p.x+W/2.f, p.y }, { p.x+W/2.f, p.y+btnH },
                ImGui::ColorConvertFloat4ToU32(cfg.ColorDivider), 1.5f*S);
 
    // ── Scroll wheel ──────────────────────────────────────────────────────
    float whlX = p.x + W/2.f - whlW/2.f;
    float whlY = p.y + btnH/2.f - whlH/2.f;
 
    drawScrollWheel(dl, { whlX, whlY }, whlW, whlH, whlR,
                    0.0f, isMiddleDown, cfg );
 
    // ── Reserve layout space ──────────────────────────────────────────────
    ImGui::Dummy({ W, H });
}
} // namespace onyx::input::tools
#endif
