#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>
#include <onyx/entity/entity.h>

namespace onyx {
namespace assets {
struct AssetId;
}

namespace game_core {
class GameCoreSystem;
}
} // namespace onyx

namespace onyx::editor {
struct AddComponentCommand : public SceneCommand {
    AddComponentCommand( ecs::EntityId entity,
                         StringId32 componentTypeId,
                         assets::AssetId sceneId,
                         game_core::GameCoreSystem& gameCoreSystem );

    void execute() override;

  private:
    ecs::EntityId m_entityId;
    StringId32 m_componentTypeId;
};
} // namespace onyx::editor
