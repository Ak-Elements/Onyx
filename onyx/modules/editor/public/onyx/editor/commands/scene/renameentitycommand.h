#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>

namespace onyx {
namespace ecs {
enum class EntityId : uint32_t;
}
} // namespace onyx

namespace onyx::editor {
struct RenameEntityCommand : public SceneCommand {
  public:
    RenameEntityCommand( ecs::EntityId entity,
                         String name,
                         assets::AssetId sceneId,
                         game_core::GameCoreSystem& gameCoreSystem );

    void execute() override;

  private:
    String m_Name;
    ecs::EntityId m_Entity;
};
} // namespace onyx::editor
