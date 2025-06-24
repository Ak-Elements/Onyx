#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/volume/components/volumeterraincomponent.h>

namespace Onyx::Volume::Terrain
{
    struct TerrainSettingsChanged
    {
    };

    namespace OnDataChanged
    {
        // TODO: Why do we need 2 components here? some template / tuple stuff not working
        using TerrainEntity = Entity::Entity<VolumeTerrainComponent, TerrainSettingsChanged>;

        void System(TerrainEntity terrainEntity)
        {
            auto&& [terrain] = terrainEntity.Get();

            /// upate whatever
            if (terrain.IsModified)
            {
                ONYX_LOG_INFO("Terrain settings changed");
                terrain.IsModified = false;
            }
        }
    }

    void factory(Entity::EntityRegistry& registry, Entity::EntityId entity, VolumeTerrainComponent&& volumeTerrainComponent)
    {
        registry.AddComponent<VolumeTerrainComponent>(entity, std::move(volumeTerrainComponent));
        registry.AddComponent<TerrainSettingsChanged>(entity);
    }

    void Register(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterComponent<VolumeTerrainComponent>(factory);

        ecsBuilder.RegisterSystem(OnDataChanged::System);
    }
}
