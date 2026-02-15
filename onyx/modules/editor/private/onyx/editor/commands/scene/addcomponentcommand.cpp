#include <onyx/editor/commands/scene/addcomponentcommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    AddComponentCommand::AddComponentCommand(Entity::EntityId entity, StringId32 componentTypeId, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : SceneCommand("AddComponent", sceneId, gameCoreSystem)
        , m_EntityId(entity)
        , m_ComponentTypeId(componentTypeId)
    {
    }
    
    void AddComponentCommand::Execute()
    {
        const Entity::ComponentFactory& componentFactory = GetComponentFactory();
        Entity::EntityRegistry& registry = GetScene().GetRegistry();
        componentFactory.TryCreateComponent(registry, m_EntityId, m_ComponentTypeId);
    }
}