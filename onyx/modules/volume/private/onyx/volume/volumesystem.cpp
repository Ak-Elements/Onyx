#include <onyx/volume/volumesystem.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

#include <onyx/volume/components/volumecomponent.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/graphics/generatemeshpass.h>
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

    void VolumeSystem::Init(GameCore::GameCoreSystem& gameCore)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<CreateVolumeMesh>("Volume/Create Volume Mesh");
        Graphics::RenderGraphNodeFactory::RegisterNode<GenerateVolumeMesh>("Volume/Generate Volume Mesh");

        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        RegisterSystems(ecsBuilder);
        Terrain::Register(ecsBuilder);

        ecsBuilder.RegisterComponent<VolumeComponent>();
        ecsBuilder.RegisterComponent<VolumeSourceComponent>();
        
    }
}
