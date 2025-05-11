#include <onyx/volume/volumesystem.h>
#include <onyx/assets/assetsystem.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/volume/components/volumecomponent.h>

namespace Onyx::Volume
{
    namespace
    {
        void RegisterSystems(Entity::EntityComponentSystemsGraph& graph)
        {
            graph.Register(VolumeSource::system);
            graph.Register(VolumeRendering::system);
        }
    }

    void VolumeSystem::Init(GameCore::GameCoreSystem& gameCore)
    {
        RegisterSystems(gameCore.GetECSGraph());

        Entity::EntityRegistry::RegisterComponent<VolumeComponent>();
        Entity::EntityRegistry::RegisterComponent<VolumeSourceComponent>();
    }
}
