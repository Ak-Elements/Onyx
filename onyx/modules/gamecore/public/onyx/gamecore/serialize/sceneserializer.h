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

    struct SceneSerializer : public Assets::AssetSerializer
    {
        static constexpr Array<StringView, 1> Extensions { "oscene" };

        // remove
        SceneSerializer(GameCoreSystem& gameCoreSystem, Assets::AssetSystem& assetSystem)
            : AssetSerializer(assetSystem)
            , m_GameCoreSystem(gameCoreSystem)
        {
        }

        friend class SceneSectorStreamer;

        bool Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, Serializer& serializer) const override;
        bool Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const override;
        
    private:
        bool SerializeEntity(Serializer& serializer, const Entity::EntityRegistry& registry, Entity::EntityId entityId) const;
        bool DeserializeEntity(const Deserializer& deserializer, Entity::EntityRegistry& registry, Entity::EntityId entityId) const;

        bool SerializeSectorsToJson(const Entity::EntityRegistry& registry, const DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool SerializeSectorToJson(const Entity::EntityRegistry& registry, const SceneSector& sector, const FileSystem::Filepath& sectorDirectoryPath) const;

        bool DeserializeSectorsFromJson(Scene& scene, DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const;
        bool DeserializeSectorFromJson(Scene& scene, SceneSector& outSector, const FileSystem::Filepath& sectorFilePath) const;

    private:
        GameCoreSystem& m_GameCoreSystem;
    };
}
