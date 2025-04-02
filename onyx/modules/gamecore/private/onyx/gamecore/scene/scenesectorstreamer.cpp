#include <onyx/gamecore/scene/scenesectorstreamer.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/serialize/sceneserializer.h>
#include <onyx/gamecore/scene/scenesector.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/filesystem/onyxfile.h>

#include <entt/entity/entity.hpp>

namespace Onyx::GameCore
{
    SceneSectorStreamer::SceneSectorStreamer(Scene& scene)
        : m_Scene(&scene)
    {
    }

    void SceneSectorStreamer::Update(const Vector3f& loadCenter)
    {
        // TODO: if entities change position this would need to adapt
        // This only works for static entities currently
        for (SceneSector& sector : m_Sectors)
        {
            for (SectorEntity& sectorEntity : sector.Entities)
            {
                const onyxF64 distance = (loadCenter - sectorEntity.Position).LengthSquared();

                bool isInStreamDistance = (distance - sectorEntity.BoundsRadiusSquared) < m_CloseStreamInDistance;
                bool isStreamOutDistance = (distance - sectorEntity.BoundsRadiusSquared) > m_CloseStreamOutDistance;
                if (isInStreamDistance && sectorEntity.Entity == entt::null)
                {
                    LoadSectorEntity(sector, sectorEntity);
                }
                else if (isStreamOutDistance && (sectorEntity.Entity != entt::null))
                {
                    UnloadSectorEntity(sector, sectorEntity);
                }
            }
        }
    }

    void SceneSectorStreamer::AddEntity(Entity::EntityId entity)
    {
        if (m_Sectors.empty())
        {
            // emplace 0,0,0 sector
            m_Sectors.emplace_back();
        }

        auto it = std::find_if(m_Sectors[0].Entities.begin(), m_Sectors[0].Entities.end(), [&](const SectorEntity& sectorEntity)
            {
                return sectorEntity.Entity == entity;
            });

        if (it != m_Sectors[0].Entities.end())
            return;

        const Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        const TransformComponent& transformComponent = registry.GetComponent<TransformComponent>(entity);

        SectorEntity newSectorEntity;
        newSectorEntity.Entity = entity;
        newSectorEntity.Position = transformComponent.GetTranslation();
        newSectorEntity.BoundsRadius = 1;
        newSectorEntity.BoundsRadiusSquared = 1;
        m_Sectors[0].Entities.push_back(newSectorEntity);
    }

    void SceneSectorStreamer::RemoveEntity(Entity::EntityId entity)
    {
        std::erase_if(m_Sectors[0].Entities, [&](const SectorEntity& sectorEntity)
        {
            return sectorEntity.Entity == entity;
        });
    }

    void SceneSectorStreamer::LoadSectorEntity(const SceneSector& sector, SectorEntity& entity)
    {
        ONYX_UNUSED(sector);
        ONYX_UNUSED(entity);
        /*Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        const FileSystem::JsonValue& entityJsonData = sector.m_EntitiesData[entity.EntityDataPosition];

        */
    }

    void SceneSectorStreamer::UnloadSectorEntity(SceneSector& sector, SectorEntity& entity)
    {
        ONYX_UNUSED(sector);
        ONYX_UNUSED(entity);
        /*Entity::EntityRegistry& registry = m_Scene->GetRegistry();
        registry.DeleteEntity(entity.Entity);
        entity.Entity = entt::null;*/
    }
}
