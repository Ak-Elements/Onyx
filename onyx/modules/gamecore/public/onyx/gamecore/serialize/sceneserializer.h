#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/filestream.h>

#include <onyx/entity/entity.h>

namespace onyx::game_core {
class GameCoreSystem;
}

namespace onyx::ecs {
class ComponentFactory;
class EntityRegistry;
} // namespace onyx::ecs

namespace onyx::file_system {
struct JsonValue;
}

namespace onyx::game_core {
struct SceneSector;
class Scene;

struct SceneSerializer : public assets::AssetSerializer< Scene > {
    static constexpr Array< StringView, 1 > Extensions{ "oscene" };

    friend class SceneSectorStreamer;

    bool serialize( const assets::AssetHandle< assets::AssetInterface >& asset,
                    const assets::AssetMetaData& meta,
                    Serializer& serializer,
                    const IEngine& engine ) const override;
    bool deserialize( assets::AssetHandle< assets::AssetInterface >& asset,
                      const assets::AssetMetaData& meta,
                      const Deserializer& deserializer,
                      IEngine& engine ) const override;

  private:
    bool serializeEntity( Serializer& serializer,
                          const ecs::EntityRegistry& registry,
                          const ecs::ComponentFactory& componentFactory,
                          ecs::EntityId entityId ) const;
    bool deserializeEntity( const Deserializer& deserializer,
                            ecs::EntityRegistry& registry,
                            const ecs::ComponentFactory& componentFactory,
                            ecs::EntityId entityId ) const;

    bool serializeSectorsToJson( const ecs::EntityRegistry& registry,
                                 const ecs::ComponentFactory& componentFactory,
                                 const DynamicArray< SceneSector >& sectors,
                                 const FilePath& sectorDirectoryPath ) const;
    bool serializeSectorToJson( const ecs::EntityRegistry& registry,
                                const ecs::ComponentFactory& componentFactory,
                                const SceneSector& sector,
                                const FilePath& sectorDirectoryPath ) const;

    bool deserializeSectorsFromJson( Scene& scene,
                                     const ecs::ComponentFactory& componentFactory,
                                     DynamicArray< SceneSector >& sectors,
                                     const FilePath& sectorDirectoryPath ) const;
    bool deserializeSectorFromJson( Scene& scene,
                                    const ecs::ComponentFactory& componentFactory,
                                    SceneSector& outSector,
                                    const FilePath& sectorFilePath ) const;
};
} // namespace onyx::game_core
