#include <onyx/volume/systems/volumerendersystem.h>

#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/volume/components/volumeterraincomponent.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace Onyx::Volume::Rendering
{
    // TODO: Material component should be read access
    using VolumeEntityAccess = Entity::Entity<const TerrainSettingsComponent, const Terrain::TerrainRuntimeComponent, GameCore::MaterialComponent>;
    void System(VolumeEntityAccess entity, Graphics::FrameContext& frameContext, Assets::AssetSystem& assetSytem)
    {
        GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);

        auto&& [volumeTerrainSettings, volumeTerrain, materialComponent] = entity.Get();

        if (volumeTerrain.MeshVertices == false)
            return;

        if (materialComponent.Material.IsValid() == false)
        {
            // this should be moved to the component create
            materialComponent.LoadMaterial(assetSytem);
            return;
        }

        if ((materialComponent.Material.IsValid() == false))
            return;

        Matrix4<onyxF32> transform;
        GameCore::StaticMeshIndirectDrawCall& drawCall = sceneFrameData.m_StaticMeshIndirectDrawCalls.emplace_back();
        drawCall.Transforms.emplace_back(transform);
        drawCall.VertexData = volumeTerrain.MeshVertices;

        drawCall.DrawCommandBuffer = volumeTerrain.IndirectDrawBuffer;
        drawCall.Material = materialComponent.Material;
    }

    void Register(Entity::EcsBuilder& ecsBuilder)
    {
        ecsBuilder.RegisterSystem(System);
    }
}
