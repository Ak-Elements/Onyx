#include <onyx/volume/volumemodule.h>

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
#include <onyx/volume/shadergraph/nodes/primitives/spherevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/cubevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/ellipsoidvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/primitives/planevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/differencevolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/intersectvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/operations/unionvolumeshadergraphnode.h>
#include <onyx/volume/shadergraph/nodes/volumeshadergraphoutnode.h>
#include <onyx/volume/systems/volumerendersystem.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume
{
    VolumeModule::VolumeModule()
    {
        Graphics::RenderGraphNodeFactory::RegisterNode<PreviewTerrainEditPass>();
    }

    void VolumeModule::Init(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi, GameCore::GameCoreSystem& gameCore)
    {
        Entity::EcsBuilder ecsBuilder = gameCore.GetEcsBuilder();
        Terrain::Register(ecsBuilder);
        Rendering::Register(ecsBuilder);

        Assets::AssetSystem::Register<VolumeShaderGraph, VolumeShaderGraphSerializer>(assetSystem, graphicsApi);

        Graphics::ShaderGraphNodeFactory::RegisterNode<CubeVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<EllipsoidVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<SphereVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<PlaneVolumeShaderGraphNode>();

        Graphics::ShaderGraphNodeFactory::RegisterNode<DifferenceVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<IntersectVolumeShaderGraphNode>();
        Graphics::ShaderGraphNodeFactory::RegisterNode<UnionVolumeShaderGraphNode>();

        Graphics::ShaderGraphNodeFactory::RegisterNode<VolumeShaderGraphOutNode>();

        ecsBuilder.RegisterComponent<VolumeSourceRenderDataComponent>();
        ecsBuilder.RegisterComponent<CubeComponent>();
        ecsBuilder.RegisterComponent<PlaneComponent>();
        ecsBuilder.RegisterComponent<SphereComponent>();
    }

    void VolumeModule::Shutdown()
    {
    }
}
