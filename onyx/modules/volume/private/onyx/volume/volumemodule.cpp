#include <onyx/volume/volumemodule.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>
#include <onyx/volume/components/volumesourcerenderdatacomponent.h>
#include <onyx/volume/components/csg/cubecomponent.h>
#include <onyx/volume/components/csg/planecomponent.h>
#include <onyx/volume/components/csg/spherecomponent.h>
#include <onyx/volume/graphics/previewterrainedit.h>
#include <onyx/volume/serialize/volumeshadergraphserializer.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/volume/shadergraph/nodes/sdfnoiseshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/sdfspherevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/sdfcubevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/sdfellipsoidvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/sdfplanevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/sdfdifferencevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/sdfintersectvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/sdfunionvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>
#include <onyx/volume/shadergraph/nodes/voxelpositionshadergraphnode.h>
#include <onyx/volume/systems/volumerendersystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume
{
    VolumeModule::VolumeModule(GameCore::GameCoreSystem& gameCore)
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<PreviewTerrainEditPass>();

        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        Terrain::Register(ecsBuilder);
        Rendering::Register(ecsBuilder);

        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfCubeVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfEllipsoidVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfSphereVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfPlaneVolumeShaderGraphNode>();

        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfDifferenceVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfIntersectVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfUnionVolumeShaderGraphNode>();

        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfNoise2DShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SdfNoise3DShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<GetVoxelPositionShaderGraphNode>();

        Graphics::ShaderGraphNodeFactory::RegisterNode<VolumeShaderGraphOutNode>();

        ecsBuilder.RegisterComponent<VolumeSourceRenderDataComponent>();
        ecsBuilder.RegisterComponent<CubeComponent>();
        ecsBuilder.RegisterComponent<PlaneComponent>();
        ecsBuilder.RegisterComponent<SphereComponent>();
    }

    VolumeModule::~VolumeModule() = default;
}
