#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/assethandle.h>
#include <onyx/entity/entity.h>

namespace onyx::assets {
struct AssetId;

}

namespace onyx::input_actions {
struct InputActionEvent;
}

namespace onyx::game_core {
class Scene;
class GameCoreSystem;
} // namespace onyx::game_core

namespace onyx::ecs {
class EntityRegistry;
enum class EntityId : uint32_t;
} // namespace onyx::ecs

namespace onyx::editor {
class ICommandGraph;
}

namespace onyx::editor::scene_editor {
class EntitiesPanel : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "EntitiesPanel";
    static constexpr StringView WindowCategory = "Panel";

    void SetCommandGraph( ICommandGraph& commandGraph ) { m_CommandGraph = &commandGraph; }

  private:
    void OnOpen() override;
    void OnClose() override;

    void OnRender( ui::ImGuiSystem& imguiSystem ) override;

    void OnDeleteAction( const input_actions::InputActionEvent& deleteAction );

  private:
    String GetNewEntityName() const;

    void DeleteEntity( ecs::EntityId entity );
    void SetSelectedEntity( ecs::EntityId entity );

  private:
    ecs::EntityId m_SelectedEntity = ecs::EntityId::Invalid;

    ICommandGraph* m_CommandGraph = nullptr;
};
} // namespace onyx::editor::scene_editor
