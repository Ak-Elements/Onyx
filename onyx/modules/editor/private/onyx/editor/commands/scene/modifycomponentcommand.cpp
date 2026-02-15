#include <onyx/editor/commands/scene/modifycomponentcommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    ModifyComponentCommand::ModifyComponentCommand(Entity::EntityId entity, StringId32 componentTypeId, DynamicArray<onyxU32>&& componentData, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : SceneCommand("ModifyComponent", sceneId, gameCoreSystem)
        , m_ComponentData(std::move(componentData))
        , m_EntityId(entity)
        , m_ComponentTypeId(componentTypeId)
    {
    }
    
    void ModifyComponentCommand::Execute()
    {
        const Entity::ComponentFactory& componentFactory = GetComponentFactory();
        Entity::EntityRegistry& registry = GetScene().GetRegistry();

        const Entity::IComponentMeta* componentMeta = componentFactory.GetComponentMeta(m_ComponentTypeId).value_or(nullptr);
        ONYX_ASSERT(componentMeta != nullptr);

        if (entt::basic_sparse_set<Entity::EntityId>* componentStorage = registry.GetStorage(componentMeta->GetRuntimeTypeId()))
        {
            void* componentPtr = componentStorage->value(m_EntityId);
            if (componentPtr == nullptr)
                return;

            bool hasCopied = componentFactory.TryCreateComponent(registry, m_EntityId, m_ComponentTypeId, m_ComponentData);
            if (hasCopied == false)
            {
                ONYX_LOG_WARNING("Failed modifying component({}) on entity {}", m_ComponentTypeId, static_cast<onyxU32>(m_EntityId));
            }
        }
    }
}