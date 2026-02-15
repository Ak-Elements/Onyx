#include <onyx/editor/commands/scene/renameentitycommand.h>

#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    RenameEntityCommand::RenameEntityCommand(Entity::EntityId entity, String name, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : SceneCommand("RenameEntity", sceneId, gameCoreSystem)
        , m_Name(name)
        , m_Entity(entity)
    {
    }

    void RenameEntityCommand::Execute()
    {
        Entity::EntityRegistry& registry = GetScene().GetRegistry();
        registry.AddComponent<GameCore::NameComponent>(m_Entity, m_Name);
    }
}