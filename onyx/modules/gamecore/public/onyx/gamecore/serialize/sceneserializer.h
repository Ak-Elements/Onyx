#pragma once
#include <onyx/assets/assetserializer.h>
#include <onyx/filesystem/filestream.h>

#include <onyx/entity/entity.h>

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx::GameCore
{
    struct SceneSector;
    class Scene;

    struct SceneSerializer : public Assets::AssetSerializer
    {
        static constexpr Tuple Extensions = { "oscene" };

        // remove
        SceneSerializer(Assets::AssetSystem& assetSystem)
            : AssetSerializer(assetSystem)
        {
        }

        friend class SceneSectorStreamer;

        bool Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;
        bool SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& sceneFilePath) const override;
        bool SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const override;

        bool Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;
        bool DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& sceneFilePath) const override;
        bool DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const override;

    private:
        bool SerializeEntity(const Entity::EntityRegistry& registry, Entity::EntityId entityId, FileSystem::JsonValue& outEntityJsonObj) const;
        bool DeserializeEntity(Entity::EntityRegistry& registry, Entity::EntityId entityId, const FileSystem::JsonValue& entityJson) const;

        bool SerializeSectorsToJson(const Entity::EntityRegistry& registry, const DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool SerializeSectorToJson(const Entity::EntityRegistry& registry, const SceneSector& sector, const FileSystem::Filepath& sectorDirectoryPath) const;

        bool DeserializeSectorsFromJson(Scene& scene, DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool DeserializeSectorFromJson(Scene& scene, SceneSector& outSector, const FileSystem::Filepath& sectorFilePath) const;
    };
}
