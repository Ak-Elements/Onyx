#include <onyx/assets/assetsystem.h>
#include <onyx/gamecore/serialize/sceneserializer.h>

#include <onyx/entity/entity.h>
#include <onyx/entity/componentmeta.hpp>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/jsonserializer.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/components/transientcomponent.gen.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx::game_core
{
    bool SceneSerializer::Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, Serializer& serializer, const IEngine& engine) const
    {
        const Scene& scene = asset.As<Scene>();

        serializer.Write<"renderGraph">(scene.m_SceneRenderGraph.GetId());

        // Serialize each sector
        const SceneSectorStreamer& sectorStreamer = scene.m_SectorStreamer;
        const DynamicArray<SceneSector>& sectors = sectorStreamer.m_Sectors;

        const GameCoreSystem& gameCoreSystem = engine.GetSystem<GameCoreSystem>();
        const ecs::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();

        bool hasSucceeded = SerializeSectorsToJson(scene.m_Registry, componentFactory, sectors, file_system::Path::GetFullPath(meta.Path).parent_path());

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeSectorsToJson(const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, const DynamicArray<SceneSector>& sectors, const FilePath& sectorDirectoryPath) const
    {
        bool hasSucceeded = true;

        for (const SceneSector& sceneSector : sectors)
        {
            hasSucceeded &= SerializeSectorToJson(registry, componentFactory, sceneSector, sectorDirectoryPath);
        }

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeSectorToJson(const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, const SceneSector& sector, const FilePath& sectorDirectoryPath) const
    {
        file_system::JsonSerializer serializer;
        serializer.WriteForEach(sector.Entities, [&](Serializer& scopeSerializer, const SectorEntity& sectorEntity)
            {
                if ((sectorEntity.Entity != entt::null) &&
                    (registry.HasComponents<TransientComponent>(sectorEntity.Entity) == false))
                {
                    return scopeSerializer.Write<"position">(sectorEntity.Position) &&
                        scopeSerializer.Write<"radius">(sectorEntity.BoundsRadius) &&
                        SerializeEntity(scopeSerializer, registry, componentFactory, sectorEntity.Entity);
                }

                return true;
            });

        FilePath sectorFilePath = sectorDirectoryPath;
        sectorFilePath.append(format::Format("{}_{}_{}", sector.Position[0], sector.Position[1], sector.Position[2]));
        sectorFilePath.replace_extension("osector");

        using namespace file_system;
        OnyxFile sceneFile(sectorFilePath);
        FileStream outStream = sceneFile.OpenStream(OpenMode::Write | OpenMode::Text);

        const String& jsonString = serializer.JsonRoot.dump(4);
        outStream.WriteRaw(jsonString.data(), jsonString.size());

        return true;
    }


    bool SceneSerializer::Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& engine) const
    {
        assets::AssetSystem& assetSystem = engine.GetSystem<assets::AssetSystem>();

        Scene& scene = asset.As<Scene>();
        ecs::EntityRegistry& registry = scene.GetRegistry();
        registry.Clear();

        assets::AssetId renderGraphAssetId = "engine:/rendergraphs/default.orendergraph";
        deserializer.Read<"renderGraph">(renderGraphAssetId);
        assetSystem.GetAsset(renderGraphAssetId, scene.m_SceneRenderGraph);

        FilePath sceneDirectoryPath = file_system::Path::GetFullPath(meta.Path.parent_path());
        SceneSectorStreamer& sectorStreamer = scene.m_SectorStreamer;
        DynamicArray<SceneSector>& sectors = sectorStreamer.m_Sectors;
        const GameCoreSystem& gameCoreSystem = engine.GetSystem<GameCoreSystem>();
        const ecs::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();
        bool hasSucceeded = DeserializeSectorsFromJson(scene, componentFactory, sectors, sceneDirectoryPath);

        return hasSucceeded;
    }

    bool SceneSerializer::DeserializeSectorsFromJson(Scene& scene, const ecs::ComponentFactory& componentFactory, DynamicArray<SceneSector>& sectors, const FilePath& sectorDirectoryPath) const
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
                    DeserializeSectorFromJson(scene, componentFactory, sector, entry.path());
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeSectorFromJson(Scene& scene, const ecs::ComponentFactory& componentFactory, SceneSector& outSector, const FilePath& sectorFilePath) const
    {
        bool hasSucceeded = true;

        file_system::OnyxFile sectorFile(sectorFilePath);
        const file_system::JsonValue& sectorJson = sectorFile.LoadJson();
        file_system::JsonDeserializer deserializer(sectorJson.Json);

        deserializer.ReadForEach(outSector.Entities, [&](const Deserializer& scopeDeserializer, SectorEntity& outEntity)
        {
                scopeDeserializer.Read<"position">(outEntity.Position);
                scopeDeserializer.Read<"radius">(outEntity.BoundsRadius);
                outEntity.BoundsRadiusSquared = outEntity.BoundsRadius * outEntity.BoundsRadius;

                outEntity.Entity = scene.GetRegistry().CreateEntity();
                return DeserializeEntity(scopeDeserializer, scene.GetRegistry(), componentFactory, outEntity.Entity);
        });

        return hasSucceeded;
    }

    bool SceneSerializer::SerializeEntity(Serializer& serializer, const ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, ecs::EntityId entityId) const
    {
        // iterate all component storages and save out the components for the entity
        onyxU32 index = 0;

        for (auto componentStorageIt : registry.GetStorage())
        {
            // if the component storage contains the entity we know that the entity has this component
            if (const entt::basic_sparse_set<ecs::EntityId>& componentStorage = componentStorageIt.second; componentStorage.contains(entityId))
            {
                entt::id_type runtimeTypeId = componentStorageIt.first;
                
                if (const ecs::IComponentMeta* meta = componentFactory.GetComponentMeta(runtimeTypeId).value_or(nullptr))
                {
                    if (meta->IsTransient())
                        continue;

                    SerializationScope scope = serializer.EnterScope<"components">();
                    {
                        SerializationScope arrayIndexScope = serializer.EnterScope(index++);

                        const StringId32 typeId = meta->GetTypeId();
                        serializer.Write<"typeId">(typeId);
                        if (meta->IsFlag() == false)
                        {
                            meta->Serialize(componentStorage.value(entityId), serializer);
                        }
                    }
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeEntity(const Deserializer& deserializer, ecs::EntityRegistry& registry, const ecs::ComponentFactory& componentFactory, ecs::EntityId entityId) const
    {
        return deserializer.ReadForEach<"components">([&](const Deserializer& scopeDeserializer)
        {
            StringId32 componentTypeId;
            scopeDeserializer.Read<"typeId">(componentTypeId);

            bool hasCreated = componentFactory.TryCreateComponent(registry, entityId, componentTypeId, scopeDeserializer);
            if (hasCreated == false)
            {
                ONYX_LOG_WARNING("Failed deserializing component. Unkown component {}", componentTypeId);
            }

            return true;
        });
    }
}
