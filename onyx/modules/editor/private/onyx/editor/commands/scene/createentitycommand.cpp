#include <onyx/editor/commands/scene/createentitycommand.h>

#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    CreateEntityCommand::CreateEntityCommand(Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : SceneCommand("CreateEntity", sceneId, gameCoreSystem)
    {
    }

    void CreateEntityCommand::Execute()
    {
        Entity::EntityRegistry& registry = GetScene().GetRegistry();
        
        if (m_EntityId == Entity::EntityId::Invalid)
        {
            m_EntityId = registry.CreateEntity();
        }
        else
        {
            std::ignore = registry.CreateEntity(m_EntityId);
        }

        registry.AddComponent<GameCore::IdComponent>(m_EntityId);
        registry.AddComponent<GameCore::NameComponent>(m_EntityId, "New Entity");
        registry.AddComponent<GameCore::TransformComponent>(m_EntityId);
    }
}