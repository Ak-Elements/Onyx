#include <onyx/nodegraph/graphrunner.h>
#include <onyx/physics/collisionlayers.h>
#include <onyx/physics/motiontype.h>
#include <onyx/volume/graphics/volumeterrainpass.h>
#include <onyx/volume/source/sdfsample.h>
#include <onyx/volume/systems/volumeterrainsystem.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/freecameracomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/rhi/commandbuffer.h>
#include <onyx/rhi/graphicssystem.h>
#include <onyx/volume/components/volumeterraincomponent.gen.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>
#include <onyx/volume/terrain/worldsparseoctreenode.h>

// TODO: remove
#include <onyx/volume/source/csg/csgplane.h>
#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/volume/source/csg/operations/csgunion.h>

#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/graphics/textureasset.h>
#include <onyx/physics/physicsworld3d.h>

// CPU terrain function <-

// Texture array / terrain material on content component
// Restore sculpting of terrain ( terrain edit panel )
//
//  CodeGen issue with new components ( there is some weird bug when adding new components the codegen does not pick
//  them up )

namespace onyx::volume::terrain {
namespace {

onyx::assets::AssetHandle< graphics::TextureAsset > locTexture0;
onyx::assets::AssetHandle< graphics::TextureAsset > locTexture1;
onyx::assets::AssetHandle< graphics::TextureAsset > locTexture2;

Vector4f32 evaluateTerrainSdf( const onyx::assets::AssetHandle< VolumeShaderGraph >& volumeGraph,
                               Vector3f32& position ) {
    if( volumeGraph.isValid() ) {
        node_graph::GraphRunner runner( volumeGraph->getNodeGraph() );

        Vector3f32& worldPos = runner.getContext().get< Vector3f32 >();
        worldPos = position;

        runner.prepare();
        runner.update( 0 );

        SdfSample sdfSample = runner.getContext().get< SdfSample >();
        return Vector4f32{ sdfSample.Gradient, sdfSample.Distance };
    } else {
        CSGPlane plane( 0.0f, Vector3f32{ 0.0f, 1.0f, 0.0f } );
        CSGSphere sphere( 500.0f, Vector3f32{ 0.0f, 0.0f, -1000.0f } );
        CSGUnion csgUnion( &sphere, &plane );
        return csgUnion.getValueAndGradient( position );
    }
}
} // namespace

namespace init {

using TerrainAccess = ecs::Access::Read< TerrainSettingsComponent >::With<
    InitTerrainFlag >::Write< VolumeGenerationComponent, TerrainWorldOctreeComponent, TerrainRuntimeComponent >;
//
using TerrainEntity = TerrainAccess::AsEntity;

void loadShader( assets::AssetSystem& assetSystem,
                 rhi::GraphicsSystem& graphicsSystem,
                 const TerrainSettingsComponent& terrainSettings,
                 VolumeGenerationComponent& generationComponent ) {
    if( generationComponent.UpdateWorldOctreeShader != nullptr )
        return;

    if( generationComponent.HasLoadedShaders )
        return;

    if( terrainSettings.VolumeGraph.hasAssetId() ) {
        bool isAssetAvailable = true;
        if( ( generationComponent.VolumeGraph.isValid() == false ) ) {
            generationComponent.HasLoadedShaders = false;
            isAssetAvailable = assetSystem.getAsset( terrainSettings.VolumeGraph.getId(),
                                                     generationComponent.VolumeGraph );
        }

        if( isAssetAvailable ) {
            if( ( generationComponent.VolumeGraph.isValid() == false ) || generationComponent.VolumeGraph->isLoading() )
                return;
        }
    }

    rhi::PipelineProperties properties;

    if( generationComponent.VolumeGraph.hasAssetId() ) {
        properties.Shader = generationComponent.VolumeGraph->getRenderTerrainShader();

    } else {
        properties.Shader = assetSystem.resolveAssetId( "engine:/shaders/volume/render_terrain.slang" );
    }

    rhi::RenderPassSettings renderPassSettings;
    rhi::RenderPassSettings::Subpass& subpass = renderPassSettings.m_SubPasses.emplace();
    rhi::RenderPassSettings::Attachment attachment{};

    attachment.m_Format = enums::toIntegral( rhi::TextureFormat::RGBA_FLOAT32 );
    attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::DontCare );
    renderPassSettings.m_Attachments.add( attachment );

    attachment.m_Format = enums::toIntegral( rhi::TextureFormat::DEPTH_FLOAT32 );
    attachment.m_LoadOp = enums::toIntegral( rhi::RenderPassSettings::LoadOp::Load );
    renderPassSettings.m_Attachments.add( attachment );

    subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::RenderTarget );
    subpass.m_AttachmentAccesses.emplace( rhi::RenderPassSettings::AttachmentAccess::DepthReadStencilRead );

    properties.RenderPass = graphicsSystem.getOrCreateRenderPass( renderPassSettings );

    properties.Rasterization.CullMode = rhi::CullMode::None;
    properties.DepthStencil.IsDepthEnabled = true;
    properties.DepthStencil.IsDepthWriteEnabled = true;
    properties.DepthStencil.IsStencilEnabled = false;
    properties.DepthStencil.Compare = rhi::CompareOperation::LessOrEqual;

    rhi::BlendState& blendState = properties.BlendStates.emplace();
    blendState.SourceColor = rhi::Blend::SrcAlpha;
    blendState.DestinationColor = rhi::Blend::OneMinusSrcAlpha;
    blendState.ColorOperation = rhi::BlendOperation::Add;
    blendState.SourceAlpha = rhi::Blend::SrcAlpha;
    blendState.DestinationAlpha = rhi::Blend::OneMinusSrcAlpha;
    blendState.AlphaOperation = rhi::BlendOperation::Add;

    generationComponent.RenderTerrainShader = graphicsSystem.createShaderInstance( properties.Shader, properties );

    assetSystem.getAsset( assetSystem.resolveAssetId( "project:/textures/ground/dirtsticks/dirtsticks_basecolor.png" ),
                          locTexture0 );
    assetSystem.getAsset( assetSystem.resolveAssetId( "project:/textures/ground/dirtsticks/dirtsticks_normal.png" ),
                          locTexture1 );
    assetSystem.getAsset( assetSystem.resolveAssetId( "project:/textures/ground/dirtsticks/dirtsticks_height.png" ),
                          locTexture2 );
    generationComponent.HasLoadedShaders = true;
}

void system( TerrainEntity terrainEntity,
             assets::AssetSystem& assetSystem,
             rhi::GraphicsSystem& graphicsSystem,
             game_core::GameCoreSystem& gameCoreSystem,
             onyx::physics::PhysicsWorld3d& physicsWorld,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ terrainSettings, generationComponent, terrainWorldOctree, terrainRuntime ] = terrainEntity;

    loadShader( assetSystem, graphicsSystem, terrainSettings, generationComponent );

    if( ( generationComponent.HasLoadedShaders == false ) || !generationComponent.RenderTerrainShader.isValid() ||
        !locTexture0.isValid() || !locTexture1.isValid() || !locTexture2.isValid() )
        return;

    // TODO: Fix amount of max sources (100) and data size (vec4)
    rhi::BufferProperties ssboVolumeSourceListProps;
    ssboVolumeSourceListProps.m_DebugName = "Volume-SourcesList";
    ssboVolumeSourceListProps.m_Size = sizeof( uint64_t ) * 100;
    ssboVolumeSourceListProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                     rhi::BufferUsage::DeviceAddress );
    ssboVolumeSourceListProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboVolumeSourceListProps.m_IsWritable = true;
    graphicsSystem.createBuffer( terrainWorldOctree.VolumeObjects, ssboVolumeSourceListProps );

    rhi::BufferProperties ssboVolumeSourcesDataProps;
    ssboVolumeSourcesDataProps.m_DebugName = "Volume-SourcesData";
    ssboVolumeSourcesDataProps.m_Size = sizeof( Vector4f32 ) * 100;
    ssboVolumeSourcesDataProps.m_UsageFlags = static_cast< uint8_t >( rhi::BufferUsage::Storage |
                                                                      rhi::BufferUsage::DeviceAddress );
    ssboVolumeSourcesDataProps.m_GpuAccess = rhi::GPUAccess::Write;
    ssboVolumeSourcesDataProps.m_IsWritable = true;
    graphicsSystem.createBuffer( terrainWorldOctree.VolumeObjectsData, ssboVolumeSourcesDataProps );

    auto sdfFunctor = [ volumeGraph = generationComponent.VolumeGraph ]( Vector3f32 position ) {
        return evaluateTerrainSdf( volumeGraph, position );
    };

    onyx::physics::BodyId bodyId = physicsWorld.createSdfCollider( Vector3f32::zero(),
                                                                   Rotor3f32{},
                                                                   std::move( sdfFunctor ),
                                                                   physics::MotionType::Static,
                                                                   physics::CollisionLayer::Static );

    entityCommandBuffer.addComponent< onyx::game_core::physics::components::BodyId >( terrainEntity.getId(), bodyId );

    // use texture 'atlas' instead of 3 seperate textures
    game_core::Scene& scene = *gameCoreSystem.getScene();
    graphics::RenderGraph& renderGraph = scene.getRenderGraph();

    VolumeTerrainInstance& instance = renderGraph.getInput< VolumeTerrainInstance >();
    instance.Shader = generationComponent.RenderTerrainShader;
    instance.VolumeSources = terrainWorldOctree.VolumeObjects;
    instance.VolumeSourcesData = terrainWorldOctree.VolumeObjectsData;
    instance.TextureIndex0 = locTexture0->getTextureHandle().Texture->GetIndex();
    instance.TextureIndex1 = locTexture1->getTextureHandle().Texture->GetIndex();
    instance.TextureIndex2 = locTexture2->getTextureHandle().Texture->GetIndex();

    entityCommandBuffer.removeComponent< InitTerrainFlag >( terrainEntity.getId() );
}

} // namespace init

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, TerrainSettingsComponent&& volumeTerrainComponent ) {
    // compare if resolution or chunksize changed if entity already has the component
    registry.addComponent< TerrainSettingsComponent >( entity, std::move( volumeTerrainComponent ) );
    registry.addComponent< TerrainRuntimeComponent >( entity );
    registry.addComponent< VolumeGenerationComponent >( entity );
    registry.addComponent< TerrainWorldOctreeComponent >( entity );

    registry.addComponent< InitTerrainFlag >( entity );
}

void Register( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.registerComponent< TerrainSettingsComponent >( factory );

    ecsBuilder.registerSystem( init::system );
    // ecsBuilder.registerSystem( render::system );
}
} // namespace onyx::volume::terrain
