#include <onyx/editor/commands/scene/createentitycommand.h>

#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor
{
    CreateEntityCommand::CreateEntityCommand(assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem)
        : SceneCommand("CreateEntity", sceneId, gameCoreSystem)
    {
    }

    void CreateEntityCommand::Execute()
    {
        ecs::EntityRegistry& registry = GetScene().GetRegistry();
        
        if (m_EntityId == ecs::EntityId::Invalid)
        {
            m_EntityId = registry.CreateEntity();
        }
        else
        {
            std::ignore = registry.CreateEntity(m_EntityId);
        }

        registry.AddComponent<game_core::IdComponent>(m_EntityId);
        registry.AddComponent<game_core::NameComponent>(m_EntityId, "New Entity");
        registry.AddComponent<game_core::TransformComponent>(m_EntityId);
    }
}