#include <onyx/editor/commands/scene/renameentitycommand.h>

#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor
{
    RenameEntityCommand::RenameEntityCommand(ecs::EntityId entity, String name, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem)
        : SceneCommand("RenameEntity", sceneId, gameCoreSystem)
        , m_Name(name)
        , m_Entity(entity)
    {
    }

    void RenameEntityCommand::Execute()
    {
        ecs::EntityRegistry& registry = GetScene().GetRegistry();
        registry.AddComponent<game_core::NameComponent>(m_Entity, m_Name);
    }
}