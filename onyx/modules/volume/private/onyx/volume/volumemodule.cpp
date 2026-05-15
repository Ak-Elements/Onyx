#include <onyx/volume/volumemodule.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/volume/components/csg/cubecomponent.gen.h>
#include <onyx/volume/components/csg/planecomponent.gen.h>
#include <onyx/volume/components/csg/spherecomponent.gen.h>

#include <onyx/volume/systems/volumerendersystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace onyx::volume {
VolumeModule::VolumeModule( game_core::GameCoreSystem& gameCore ) {
    ecs::EcsBuilder ecsBuilder = gameCore.getEcsBuilder();
    terrain::Register( ecsBuilder );
    rendering::Register( ecsBuilder );

    ecsBuilder.RegisterComponent< CubeComponent >();
    ecsBuilder.RegisterComponent< PlaneComponent >();
    ecsBuilder.RegisterComponent< SphereComponent >();
}

VolumeModule::~VolumeModule() = default;
} // namespace onyx::volume
