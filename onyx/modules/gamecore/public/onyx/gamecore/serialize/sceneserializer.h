#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/filestream.h>

#include <onyx/entity/entity.h>

namespace Onyx::GameCore
{
    class GameCoreSystem;
}

namespace Onyx::Entity
{
    class ComponentFactory;
    class EntityRegistry;
}

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx::GameCore
{
    struct SceneSector;
    class Scene;

    struct SceneSerializer : public Assets::AssetSerializer<Scene>
    {
        static constexpr Array<StringView, 1> Extensions { "oscene" };

        friend class SceneSectorStreamer;

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer, IEngine& engine) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const override;
        
    private:
        bool SerializeEntity(Serializer& serializer, const Entity::EntityRegistry& registry, const Entity::ComponentFactory& componentFactory, Entity::EntityId entityId) const;
        bool DeserializeEntity(const Deserializer& deserializer, Entity::EntityRegistry& registry, const Entity::ComponentFactory& componentFactory, Entity::EntityId entityId) const;

        bool SerializeSectorsToJson(const Entity::EntityRegistry& registry, const Entity::ComponentFactory& componentFactory, const DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool SerializeSectorToJson(const Entity::EntityRegistry& registry, const Entity::ComponentFactory& componentFactory, const SceneSector& sector, const FileSystem::Filepath& sectorDirectoryPath) const;

        bool DeserializeSectorsFromJson(Scene& scene, const Entity::ComponentFactory& componentFactory, DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool DeserializeSectorFromJson(Scene& scene, const Entity::ComponentFactory& componentFactory, SceneSector& outSector, const FileSystem::Filepath& sectorFilePath) const;
    };
}
