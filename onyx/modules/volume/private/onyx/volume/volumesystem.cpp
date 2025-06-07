#include <onyx/volume/volumesystem.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

#include <onyx/volume/components/volumecomponent.h>
#include <onyx/volume/graphics/generatemeshpass.h>

namespace Onyx::Volume
{
    namespace
    {
        void RegisterSystems(Entity::EntityComponentSystemsGraph& graph)
        {
            Systems::registerSystem(graph);
        }
    }

    void VolumeSystem::Init(GameCore::GameCoreSystem& gameCore)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<CreateVolumeMesh>("Volume/Create Volume Mesh");
        Graphics::RenderGraphNodeFactory::RegisterNode<GenerateVolumeMesh>("Volume/Generate Volume Mesh");

        RegisterSystems(gameCore.GetECSGraph());

        Entity::EntityRegistry::RegisterComponent<VolumeComponent>();
        Entity::EntityRegistry::RegisterComponent<VolumeSourceComponent>();
    }
}
