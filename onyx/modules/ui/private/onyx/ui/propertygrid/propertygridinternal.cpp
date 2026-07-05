
#include <onyx/ui/propertygrid/propertygridinternal.h>

#if ONYX_IS_EDITOR

#include <onyx/assets/assetsystem.h>
#include <onyx/ui/controls/assetselector.h>
#include <onyx/ui/controls/colorcontrol.h>
#include <onyx/ui/imguisystem.h>
#include <onyx/ui/scopeddisable.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/widgets.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace onyx::ui::property_grid::internal {
namespace {
Stack< ImGuiID > g_locPropertyGridIdStack;
int32_t g_locSplitterMinX;
} // namespace

ImVec2 getPropertyValuePosition() {
    float32 splitterPosX = getSplitterPositionX();

    const ImGuiStyle& style = ImGui::GetStyle();
    ::ImGuiWindow* window = ImGui::GetCurrentWindow();
    float32 indendation = window->DC.Indent.x;
    return { splitterPosX - indendation + style.DockingSeparatorSize + 2 * style.ItemInnerSpacing.x,
             ImGui::GetFrameHeightWithSpacing() };
}

int32_t getSplitterPositionX() {
    ImGuiID propertyGridID = g_locPropertyGridIdStack.top();

    ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
    uint32_t splitterId = imguiStateStorage->GetInt( propertyGridID );
    return imguiStateStorage->GetInt( splitterId, g_locSplitterMinX );
}

void setSplitterPositionX( int32_t value ) {
    ImGuiID propertyGridID = g_locPropertyGridIdStack.top();

    ImGuiStorage* imguiStateStorage = ImGui::GetStateStorage();
    uint32_t splitterId = imguiStateStorage->GetInt( propertyGridID );
    return imguiStateStorage->SetInt( splitterId, std::max( value, g_locSplitterMinX ) );
}

ImGuiID beginPropertyGrid( StringView id, int32_t splitterMinX ) {
    g_locSplitterMinX = splitterMinX;
    return g_locPropertyGridIdStack.emplace( ImGui::GetID( id.data() ) );
}

void endPropertyGrid() {
    g_locPropertyGridIdStack.pop();
}

ImGuiID getPropertyGridId() {
    return g_locPropertyGridIdStack.top();
}

bool drawPropertyValue( StringView id, StringView value ) {
    return drawPropertyValue( id, value, ImGuiInputTextFlags_None );
}

bool drawPropertyValue( StringView id, StringView value, ImGuiInputTextFlags flags ) {
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    ScopedImGuiDisabled disabled;
    return drawStringInput( format::format( "##{}", id ), value, ImVec2( 0, 0 ), flags | ImGuiInputTextFlags_ReadOnly );
}

bool drawPropertyValue( StringView id, String& value ) {
    return drawPropertyValue( id, value, ImGuiInputTextFlags_None );
}

bool drawPropertyValue( StringView id, String& value, ImGuiInputTextFlags flags ) {
    ScopedImGuiStyle style{ ImGuiStyleVar_FrameBorderSize, 1.0f };
    return drawStringInput( format::format( "##{}", id ), value, ImVec2( 0, 0 ), flags );
}

} // namespace onyx::ui::property_grid::internal
#endif
