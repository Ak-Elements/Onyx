#pragma once

#include <onyx/assets/assetid.h>
#include <onyx/editor/commands/command.h>

namespace onyx {
namespace ecs {
class ComponentFactory;
}

namespace game_core {
class GameCoreSystem;
class Scene;
} // namespace game_core
} // namespace onyx

namespace onyx::editor {
struct SceneCommand : public ICommand {
    SceneCommand( StringId32 id, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem );

  protected:
    game_core::Scene& getScene();
    const ecs::ComponentFactory& getComponentFactory();

  private:
    assets::AssetId m_sceneId = assets::AssetId::invalid();
    game_core::GameCoreSystem* m_gameCoreSystem = nullptr;
};
} // namespace onyx::editor
