#pragma once

#include <onyx/ui/imguiwindow.h>

namespace onyx {
namespace assets {
struct AssetId;
}

namespace input_actions {
struct InputActionEvent;
}

namespace game_core {
class Scene;
}
} // namespace onyx

namespace onyx::editor {
class ICommandGraph;

class SceneViewportWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "SceneViewport";
    static constexpr StringView WindowCategory = "Window";

    StringView getWindowId() override { return WindowId; }

    void setCommandGraph( ICommandGraph& commandGraph ) { m_commandGraph = &commandGraph; }

  private:
    enum class GizmoType : uint8_t { Translate, Rotate, Scale };

    void onOpen() override;
    void onClose() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void renderImGuizmo( assets::AssetId sceneId,
                         game_core::Scene& scene,
                         const Vector2f32& viewportPosition,
                         const Vector2f32& viewportExtents );

    void onGizmoModeAction( const input_actions::InputActionEvent& inputActionContext );

  private:
    ICommandGraph* m_commandGraph = nullptr;

    GizmoType m_currentGizmo = GizmoType::Translate;
    bool m_hasSelectedEntity = false;
};
} // namespace onyx::editor
