#include <onyx/gamecore/serialize/sceneserializer.h>

#include <onyx/entity/entity.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/filesystem/onyxfile.h>

namespace Onyx::GameCore
{
    bool SceneSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool SceneSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& sceneFilePath) const
    {
        const Scene& scene = asset.As<Scene>();

        using namespace FileSystem;
        OnyxFile sceneFile(sceneFilePath);
        FileStream outStream = sceneFile.OpenStream(OpenMode::Write | OpenMode::Text);

        // scene file is empty for now
        // serialize scene generic settings (e.g.: Terrain/Environment and other settings)
        JsonValue jsonRoot;
        jsonRoot.Json = nlohmann::ordered_json::object();
        
        const String& jsonString = jsonRoot.Json.dump(4);
        outStream.WriteRaw(jsonString.data(), jsonString.size());

        // Serialize each sector
        const SceneSectorStreamer& sectorStreamer = scene.m_SectorStreamer;
        const DynamicArray<SceneSector>& sectors = sectorStreamer.m_Sectors;
        bool hasSucceeded = SerializeSectorsToJson(scene.m_Registry, sectors, Path::GetFullPath(sceneFilePath).parent_path());

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool SceneSerializer::SerializeSectorsToJson(const Entity::EntityRegistry& registry, const DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const
    {
        bool hasSucceeded = true;

        for (const SceneSector& sceneSector : sectors)
        {
            hasSucceeded &= SerializeSectorToJson(registry, sceneSector, sectorDirectoryPath);
        }

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeSectorToJson(const Entity::EntityRegistry& registry, const SceneSector& sector, const FileSystem::Filepath& sectorDirectoryPath) const
    {
        FileSystem::JsonValue jsonRoot;
        jsonRoot.Json = nlohmann::ordered_json::array();

        for (const SectorEntity& sectorEntity : sector.Entities)
        {
            FileSystem::JsonValue sectorEntityJson;

            std::array<onyxF32, 3> position{ sectorEntity.Position[0], sectorEntity.Position[1], sectorEntity.Position[2] };
            sectorEntityJson.Set("position", position);
            sectorEntityJson.Set("radius", sectorEntity.BoundsRadius);

            if (sectorEntity.Entity != entt::null)
            {
                FileSystem::JsonValue entityDataJson;
                entityDataJson.Json = nlohmann::ordered_json::array();

                SerializeEntity(registry, sectorEntity.Entity, entityDataJson);
                sectorEntityJson.Set("data", entityDataJson);
            }
            // TODO: How do we handle unloaded entities in the editor?
            //else
            //{
            //    sectorEntityJson.Set("data", sector.m_EntitiesData[sectorEntity.EntityDataPosition]);
            //}

            jsonRoot.Add(sectorEntityJson);
        }

        FileSystem::Filepath sectorFilePath = sectorDirectoryPath;
        sectorFilePath.append(Format::Format("{}_{}_{}", sector.Position[0], sector.Position[1], sector.Position[2]));
        sectorFilePath.replace_extension("osector");

        using namespace FileSystem;
        OnyxFile sceneFile(sectorFilePath);
        FileStream outStream = sceneFile.OpenStream(OpenMode::Write | OpenMode::Text);

        const String& jsonString = jsonRoot.Json.dump(4);
        outStream.WriteRaw(jsonString.data(), jsonString.size());

        return true;
    }


    bool SceneSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);

        return true;
    }

    bool SceneSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& sceneFilePath) const
    {
        Scene& scene = asset.As<Scene>();
        Entity::EntityRegistry& registry = scene.GetRegistry();
        registry.Clear();

        using namespace FileSystem;
        OnyxFile sceneFile(sceneFilePath);
        const JsonValue sceneJsonRoot = sceneFile.LoadJson();
        // serialize scene generic settings (e.g.: Terrain/Environment and other settings)
        ONYX_UNUSED(sceneJsonRoot);

        SceneSectorStreamer& sectorStreamer = scene.m_SectorStreamer;
        DynamicArray<SceneSector>& sectors = sectorStreamer.m_Sectors;
        bool hasSucceeded = DeserializeSectorsFromJson(scene, sectors, sceneFilePath.parent_path());

        return hasSucceeded;
    }

    bool SceneSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);

        return true;
    }

    bool SceneSerializer::DeserializeSectorsFromJson(Scene& scene, DynamicArray<SceneSector>& sectors, const FileSystem::Filepath& sectorDirectoryPath) const
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sectorDirectoryPath))
        {
            if (entry.is_regular_file())
            {
                bool isSector = entry.path().extension() == ".osector";

                if (isSector)
                {
                    SceneSector& sector = sectors.emplace_back();
                    // TODO: Parse sector position
                    // This should not load entities
                    DeserializeSectorFromJson(scene, sector, entry.path());
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeSectorFromJson(Scene& scene, SceneSector& outSector, const FileSystem::Filepath& sectorFilePath) const
    {
        bool hasSucceeded = true;

        FileSystem::OnyxFile sectorFile(sectorFilePath);
        const FileSystem::JsonValue& sectorJsonRoot = sectorFile.LoadJson();

        for (const auto& rawSectorEntityJson : sectorJsonRoot.Json)
        {
            FileSystem::JsonValue sectorEntityJson{ rawSectorEntityJson };

            std::array<onyxF32, 3> position{ 0.0f };
            sectorEntityJson.Get("position", position);

            onyxF64 radius;
            sectorEntityJson.Get("radius", radius);

            SectorEntity& entity = outSector.Entities.emplace_back();
            entity.Position = { position[0], position[1], position[2] };
            entity.BoundsRadius = radius;
            entity.BoundsRadiusSquared = radius * radius;

            FileSystem::JsonValue entityData;
            sectorEntityJson.Get("data", entityData);

            entity.Entity = scene.GetRegistry().CreateEntity();
            SceneSerializer serializer(m_AssetSystem);
            serializer.DeserializeEntity(scene.GetRegistry(), entity.Entity, entityData);
        }

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeEntity(const Entity::EntityRegistry& registry, Entity::EntityId entityId, FileSystem::JsonValue& outEntityJsonObj) const
    {
        // iterate all component storages and save out the components for the entity
        for (auto componentStorageIt : registry.GetStorage())
        {
            // if the component storage contains the entity we know that the entity has this component
            if (const entt::basic_sparse_set<Entity::EntityId>& componentStorage = componentStorageIt.second; componentStorage.contains(entityId))
            {
                entt::meta_type metaClass = entt::resolve(componentStorageIt.first);
                if (!metaClass)
                    continue;

                const entt::meta_prop& transientProperty = metaClass.prop(Entity::TRANSIENT_PROPERTY_HASH);
                if (transientProperty.value().cast<bool>())
                    continue;

                FileSystem::JsonValue componentJsonDataObj;
                entt::meta_any componentHandle = metaClass.from_void(componentStorage.value(entityId));
                componentHandle.invoke(Entity::SERIALIZE_JSON_FUNCTION_HASH, entt::forward_as_meta(componentJsonDataObj));

                FileSystem::JsonValue componentJsonObj;
                componentJsonObj.Set("id", componentStorageIt.first);
                componentJsonObj.Set("type", metaClass.info().name());
                componentJsonObj.Set("data", componentJsonDataObj);
                outEntityJsonObj.Add(componentJsonObj);
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeEntity(Entity::EntityRegistry& registry, Entity::EntityId entityId, const FileSystem::JsonValue& entityJson) const
    {
        for (const auto& componentsJson : entityJson.Json)
        {
            FileSystem::JsonValue componentsMetaJsonObj{ componentsJson };

            entt::id_type componentId;
            componentsMetaJsonObj.Get("id", componentId);

            entt::meta_type metaClass = entt::resolve(componentId);
            if (!metaClass)
            {
                StringView componentType;
                componentsMetaJsonObj.Get("type", componentType);
                ONYX_LOG_WARNING("Entity has an unkown component with id:{}, type:{}, skipping component.", componentId, componentType);
                continue;
            }

            FileSystem::JsonValue componentJsonData;
            componentsMetaJsonObj.Get("data", componentJsonData);

            registry.AddComponent(entityId, componentId, componentJsonData);
        }

        return true;
    }
}
