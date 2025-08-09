#include <onyx/volume/volumemodule.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/volume/components/volumecomponent.h>
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
    }

    void VolumeModule::Init(GameCore::GameCoreSystem& gameCore)
    {
        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        RegisterSystems(ecsBuilder);
        Terrain::Register(ecsBuilder);

        ecsBuilder.RegisterComponent<VolumeComponent>();
        ecsBuilder.RegisterComponent<VolumeSourceComponent>();
        
    }

    void VolumeModule::Shutdown()
    {
    }
}
