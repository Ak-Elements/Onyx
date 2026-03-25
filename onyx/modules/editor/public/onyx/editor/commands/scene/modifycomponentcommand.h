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
struct ModifyComponentCommand : public SceneCommand {
    ModifyComponentCommand( ecs::EntityId entity,
                            StringId32 componentTypeId,
                            DynamicArray< uint32_t >&& componentData,
                            assets::AssetId sceneId,
                            game_core::GameCoreSystem& gameCoreSystem );

    void Execute() override;

  private:
    DynamicArray< uint32_t > m_ComponentData;

    ecs::EntityId m_EntityId;
    StringId32 m_ComponentTypeId;
};
} // namespace onyx::editor