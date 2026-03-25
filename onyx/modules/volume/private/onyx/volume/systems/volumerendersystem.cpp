#include <onyx/volume/systems/volumerendersystem.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/graphics/materialcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/sceneframedata.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

namespace onyx::volume::rendering {
// TODO: Material component should be read access
using Access = ecs::Access ::Read< TerrainSettingsComponent,
                                   terrain::TerrainRuntimeComponent >::Write< game_core::MaterialComponent >;

using TerrainEntity = Access::AsEntity;

void System( TerrainEntity entity, rhi::FrameContext& frameContext, assets::AssetSystem& assetSystem ) {
    game_core::SceneFrameData& sceneFrameData = static_cast< game_core::SceneFrameData& >( *frameContext.FrameData );

    auto&& [ volumeTerrainSettings, volumeTerrain, materialComponent ] = entity;

    if ( volumeTerrain.MeshVertices == false )
        return;

    if ( materialComponent.Material.isValid() == false ) {
        // this should be moved to the component create
        assetSystem.getAsset( materialComponent.Material.getId(), materialComponent.Material );
        return;
    }

    if ( ( materialComponent.Material.isValid() == false ) )
        return;

    Matrix4x4f32 transform;
    game_core::StaticMeshIndirectDrawCall& drawCall = sceneFrameData.m_StaticMeshIndirectDrawCalls.emplace_back();
    drawCall.Transforms.emplace_back( transform );
    drawCall.VertexData = volumeTerrain.MeshVertices;

    drawCall.DrawCommandBuffer = volumeTerrain.IndirectDrawBuffer;
    drawCall.Material = materialComponent.Material;
}

void Register( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( System );
}
} // namespace onyx::volume::rendering
