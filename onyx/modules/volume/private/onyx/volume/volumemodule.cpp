#include <onyx/volume/volumemodule.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

#include <onyx/volume/components/volumecomponent.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/graphics/generatemeshpass.h>
#include <onyx/volume/systems/terrainworldchunksystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume
{
    namespace
    {
        void RegisterSystems(Entity::EcsBuilder& ecsBuilder)
        {
            Systems::registerSystem(ecsBuilder);
        }
    }

    VolumeModule::VolumeModule()
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<CreateVolumeMesh>();
        Graphics::RenderGraphNodeFactory::RegisterNode<GenerateVolumeMesh>();
    }

    void VolumeModule::Init(GameCore::GameCoreSystem& gameCore)
    {
        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        RegisterSystems(ecsBuilder);
        Terrain::Register(ecsBuilder);
        Terrain::RegisterTerrain(ecsBuilder);

        ecsBuilder.RegisterComponent<VolumeComponent>();
        ecsBuilder.RegisterComponent<VolumeSourceComponent>();
        
    }
}
