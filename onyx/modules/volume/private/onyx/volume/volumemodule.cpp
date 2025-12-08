#include <onyx/volume/volumemodule.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/volume/components/volumesourcerenderdatacomponent.h>
#include <onyx/volume/components/csg/cubecomponent.h>
#include <onyx/volume/components/csg/planecomponent.h>
#include <onyx/volume/components/csg/spherecomponent.h>

#include <onyx/volume/systems/volumerendersystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume
{
    VolumeModule::VolumeModule(GameCore::GameCoreSystem& gameCore)
    {
        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        Terrain::Register(ecsBuilder);
        Rendering::Register(ecsBuilder);

        ecsBuilder.RegisterComponent<VolumeSourceRenderDataComponent>();
        ecsBuilder.RegisterComponent<CubeComponent>();
        ecsBuilder.RegisterComponent<PlaneComponent>();
        ecsBuilder.RegisterComponent<SphereComponent>();
    }

    VolumeModule::~VolumeModule() = default;
}
