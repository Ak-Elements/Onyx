#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/filestream.h>

#include <onyx/entity/entity.h>

namespace onyx::game_core
{
    class GameCoreSystem;
}

namespace onyx::ecs
{
    class ComponentFactory;
    class EntityRegistry;
}

namespace onyx::file_system
{
    struct JsonValue;
}

namespace onyx::game_core
{
    struct SceneSector;
    class Scene;

    struct SceneSerializer : public assets::AssetSerializer<Scene>
    {
        static constexpr Array<StringView, 1> Extensions { "oscene" };

        friend class SceneSectorStreamer;

        bool Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const override;
        bool Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
        
    private:
        bool SerializeEntity(Serializer& serializer, const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, ecs::EntityId entityId) const;
        bool DeserializeEntity(const Deserializer& deserializer, ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, ecs::EntityId entityId) const;

        bool SerializeSectorsToJson(const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, const DynamicArray<SceneSector>& sectors, const FilePath& sectorDirectoryPath) const;
        bool SerializeSectorToJson(const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, const SceneSector& sector, const FilePath& sectorDirectoryPath) const;

        bool DeserializeSectorsFromJson(Scene& scene, const ecs::ComponentFactory& componentFactory, DynamicArray<SceneSector>& sectors, const FilePath& sectorDirectoryPath) const;
        bool DeserializeSectorFromJson(Scene& scene, const ecs::ComponentFactory& componentFactory, SceneSector& outSector, const FilePath& sectorFilePath) const;
    };
}
