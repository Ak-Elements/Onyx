#include <onyx/assets/assetsystem.h>
#include <onyx/gamecore/serialize/sceneserializer.h>

#include <onyx/entity/componentmeta.hpp>
#include <onyx/entity/entity.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/jsonserializer.h>
#include <onyx/gamecore/components/transientcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx::game_core {
bool SceneSerializer::serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                                 const assets::AssetMetaData& meta,
                                 Serializer& serializer,
                                 const IEngine& engine ) const {
    const Scene& scene = asset.as< Scene >();

    serializer.write< "renderGraph" >( scene.m_sceneRenderGraph.getId() );

    // Serialize each sector
    const SceneSectorStreamer& sectorStreamer = scene.m_sectorStreamer;
    const DynamicArray< SceneSector >& sectors = sectorStreamer.m_Sectors;

    const GameCoreSystem& gameCoreSystem = engine.getSystem< GameCoreSystem >();
    const ecs::ComponentFactory& componentFactory = gameCoreSystem.getComponentFactory();

    bool hasSucceeded = serializeSectorsToJson( scene.m_registry,
                                                componentFactory,
                                                sectors,
                                                file_system::path::getFullPath( meta.Path ).parent_path() );

    return hasSucceeded;
}

bool SceneSerializer::serializeSectorsToJson( const ecs::EntityRegistry& registry,
                                              const ecs::ComponentFactory& componentFactory,
                                              const DynamicArray< SceneSector >& sectors,
                                              const FilePath& sectorDirectoryPath ) const {
    bool hasSucceeded = true;

    for( const SceneSector& sceneSector : sectors ) {
        hasSucceeded &= serializeSectorToJson( registry, componentFactory, sceneSector, sectorDirectoryPath );
    }

    return hasSucceeded;
}

bool SceneSerializer::serializeSectorToJson( const ecs::EntityRegistry& registry,
                                             const ecs::ComponentFactory& componentFactory,
                                             const SceneSector& sector,
                                             const FilePath& sectorDirectoryPath ) const {
    file_system::JsonSerializer serializer;
    serializer.writeForEach( sector.Entities, [ & ]( Serializer& scopeSerializer, const SectorEntity& sectorEntity ) {
        if( ( sectorEntity.Entity != entt::null ) &&
            ( registry.HasComponents< TransientComponent >( sectorEntity.Entity ) == false ) ) {
            return scopeSerializer.write< "position" >( sectorEntity.Position ) &&
                   scopeSerializer.write< "radius" >( sectorEntity.BoundsRadius ) &&
                   serializeEntity( scopeSerializer, registry, componentFactory, sectorEntity.Entity );
        }

        return true;
    } );

    FilePath sectorFilePath = sectorDirectoryPath;
    sectorFilePath.append(
        format::format( "{}_{}_{}", sector.Position[ 0 ], sector.Position[ 1 ], sector.Position[ 2 ] ) );
    sectorFilePath.replace_extension( "osector" );

    using namespace file_system;
    OnyxFile sceneFile( sectorFilePath );
    FileStream outStream = sceneFile.OpenStream( OpenMode::Write | OpenMode::Text );

    const String& jsonString = serializer.JsonRoot.dump( 4 );
    outStream.writeRaw( jsonString.data(), jsonString.size() );

    return true;
}

bool SceneSerializer::deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                                   const assets::AssetMetaData& meta,
                                   const Deserializer& deserializer,
                                   IEngine& engine ) const {
    assets::AssetSystem& assetSystem = engine.getSystem< assets::AssetSystem >();

    Scene& scene = asset.as< Scene >();
    ecs::EntityRegistry& registry = scene.getRegistry();
    registry.Clear();

    assets::AssetId renderGraphAssetId = "engine:/rendergraphs/default.orendergraph";
    deserializer.read< "renderGraph" >( renderGraphAssetId );
    assetSystem.getAsset( renderGraphAssetId, scene.m_sceneRenderGraph );

    FilePath sceneDirectoryPath = file_system::path::getFullPath( meta.Path.parent_path() );
    SceneSectorStreamer& sectorStreamer = scene.m_sectorStreamer;
    DynamicArray< SceneSector >& sectors = sectorStreamer.m_Sectors;
    const GameCoreSystem& gameCoreSystem = engine.getSystem< GameCoreSystem >();
    const ecs::ComponentFactory& componentFactory = gameCoreSystem.getComponentFactory();
    bool hasSucceeded = deserializeSectorsFromJson( scene, componentFactory, sectors, sceneDirectoryPath );

    return hasSucceeded;
}

bool SceneSerializer::deserializeSectorsFromJson( Scene& scene,
                                                  const ecs::ComponentFactory& componentFactory,
                                                  DynamicArray< SceneSector >& sectors,
                                                  const FilePath& sectorDirectoryPath ) const {
    for( const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator( sectorDirectoryPath ) ) {
        if( entry.is_regular_file() ) {
            bool isSector = entry.path().extension() == ".osector";
            if( isSector ) {
                SceneSector& sector = sectors.emplace_back();
                // TODO: Parse sector position
                // This should not load entities
                deserializeSectorFromJson( scene, componentFactory, sector, entry.path() );
            }
        }
    }

    return true;
}

bool SceneSerializer::deserializeSectorFromJson( Scene& scene,
                                                 const ecs::ComponentFactory& componentFactory,
                                                 SceneSector& outSector,
                                                 const FilePath& sectorFilePath ) const {
    bool hasSucceeded = true;

    file_system::OnyxFile sectorFile( sectorFilePath );
    const file_system::JsonValue& sectorJson = sectorFile.LoadJson();
    file_system::JsonDeserializer deserializer( sectorJson.Json );

    deserializer.readForEach(
        outSector.Entities,
        [ & ]( const Deserializer& scopeDeserializer, SectorEntity& outEntity ) {
            scopeDeserializer.read< "position" >( outEntity.Position );
            scopeDeserializer.read< "radius" >( outEntity.BoundsRadius );
            outEntity.BoundsRadiusSquared = outEntity.BoundsRadius * outEntity.BoundsRadius;

            outEntity.Entity = scene.getRegistry().CreateEntity();
            return deserializeEntity( scopeDeserializer, scene.getRegistry(), componentFactory, outEntity.Entity );
        } );

    return hasSucceeded;
}

bool SceneSerializer::serializeEntity( Serializer& serializer,
                                       const ecs::EntityRegistry& registry,
                                       const ecs::ComponentFactory& componentFactory,
                                       ecs::EntityId entityId ) const {
    // iterate all component storages and save out the components for the entity
    uint32_t index = 0;

    for( auto componentStorageIt : registry.GetStorage() ) {
        // if the component storage contains the entity we know that the entity has this component
        if( const entt::basic_sparse_set< ecs::EntityId >& componentStorage = componentStorageIt.second;
            componentStorage.contains( entityId ) ) {
            entt::id_type runtimeTypeId = componentStorageIt.first;

            if( const ecs::IComponentMeta* meta = componentFactory.GetComponentMeta( runtimeTypeId )
                                                      .value_or( nullptr ) ) {
                if( meta->isTransient() )
                    continue;

                SerializationScope scope = serializer.enterScope< "components" >();
                {
                    SerializationScope arrayIndexScope = serializer.enterScope( index++ );

                    const StringId32 typeId = meta->getTypeId();
                    serializer.write< "typeId" >( typeId );
                    if( meta->isFlag() == false ) {
                        meta->serialize( componentStorage.value( entityId ), serializer );
                    }
                }
            }
        }
    }

    return true;
}

bool SceneSerializer::deserializeEntity( const Deserializer& deserializer,
                                         ecs::EntityRegistry& registry,
                                         const ecs::ComponentFactory& componentFactory,
                                         ecs::EntityId entityId ) const {
    return deserializer.readForEach< "components" >( [ & ]( const Deserializer& scopeDeserializer ) {
        StringId32 componentTypeId;
        scopeDeserializer.read< "typeId" >( componentTypeId );

        bool hasCreated = componentFactory.TryCreateComponent( registry, entityId, componentTypeId, scopeDeserializer );
        if( hasCreated == false ) {
            ONYX_LOG_WARNING( "Failed deserializing component. Unkown component {}", componentTypeId );
        }

        return true;
    } );
}
} // namespace onyx::game_core
