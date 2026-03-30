#pragma once

#include "onyx/defines.h"
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

    void setCommandGraph( ICommandGraph& commandGraph ) { m_commandGraph = &commandGraph; }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void onDeleteAction( const input_actions::InputActionEvent& deleteAction );

  private:
    ONYX_NO_DISCARD String getNewEntityName() const;

    void deleteEntity( ecs::EntityId entity );
    void setSelectedEntity( ecs::EntityId entity );

  private:
    ecs::EntityId m_selectedEntity = ecs::EntityId::Invalid;

    ICommandGraph* m_commandGraph = nullptr;
};
} // namespace onyx::editor::scene_editor
