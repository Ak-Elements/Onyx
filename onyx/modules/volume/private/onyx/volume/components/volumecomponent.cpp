#include <onyx/entity/entityregistry.h>
#include <onyx/volume/components/volumecomponent.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/scene/sceneframedata.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vertex.h>

#include <onyx/volume/source/csg/csgplane.h>
#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/volume/source/noise/simplexnoisesource.h>

namespace Onyx::Volume
{
    namespace VolumeSource
    {
        constexpr onyxF32 loc_GeometricError = 0.90f;

        void OnVolumeLoaded(Graphics::GraphicsApi& api, VolumeComponent& volumeComponent, const GameCore::TransformComponent& transformComponent, const MeshBuilder& meshBuilder)
        {
            if (meshBuilder.GetVertices().empty())
                return;

            onyxU32 verticesBytes = static_cast<onyxU32>(meshBuilder.GetVertices().size() * sizeof(Graphics::Vertex));

            Graphics::BufferProperties vertexBufferProps;
            vertexBufferProps.m_DebugName = "VolumeSourceComponent Vertices";
            vertexBufferProps.m_Size = verticesBytes;
            vertexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Vertex);
            vertexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

            api.CreateBuffer(volumeComponent.Vertices, vertexBufferProps);

            DynamicArray<Graphics::Vertex> vertices;
            vertices.reserve(meshBuilder.GetVertices().size());
            for (const Vertex& vertex : meshBuilder.GetVertices())
            {
                Vector2f uv(0.0f, 0.0f);
                vertices.emplace_back(Vector3f(vertex.Position[0], vertex.Position[1], vertex.Position[2]) + transformComponent.GetTranslation(), Vector3f(vertex.Normal[0], vertex.Normal[1], vertex.Normal[2]), uv);
            }

            volumeComponent.Vertices->SetData(0, vertices.data(), verticesBytes);

            onyxU32 indicesBytes = static_cast<onyxU32>(meshBuilder.GetIndices().size() * sizeof(onyxU32));
            Graphics::BufferProperties indexBufferProps;
            indexBufferProps.m_DebugName = "VolumeSourceComponent Indices";
            indexBufferProps.m_Size = indicesBytes;
            indexBufferProps.m_BindFlags = static_cast<onyxU8>(Graphics::BufferType::Index);
            indexBufferProps.m_CpuAccess = Graphics::CPUAccess::Write;

            api.CreateBuffer(volumeComponent.Indices, indexBufferProps);
            volumeComponent.Indices->SetData(0, meshBuilder.GetIndices().data(), indicesBytes);

            volumeComponent.IsLoading = false;
        }

        void system(onyxU64 /*deltaTime*/, GameCore::Scene& scene, Graphics::GraphicsApi& api, Assets::AssetSystem&)
        {
            Entity::EntityRegistry& registry = scene.GetRegistry();
            VolumeBase* volumeSource = nullptr;
            auto csgEntities = registry.GetView<VolumeSourceComponent>();
            bool isModified = false;
            for (Entity::EntityId volumeEntity : csgEntities)
            {
                VolumeSourceComponent& volumeComponent = registry.GetComponent<VolumeSourceComponent>(volumeEntity);

                volumeSource = volumeComponent.Volume;
                if (volumeComponent.IsModified)
                {
                    isModified = true;
                    volumeComponent.IsModified = false;
                    break;
                }
            }

            if (volumeSource == nullptr)
                return;

            const onyxF32 chunkSize = 2048.0f;
            const onyxF32 sampleResolution = 0.1f;
            const onyxF32 maxDistanceSkirts = -1.0f;

            auto volumeEntities = registry.GetView<GameCore::TransformComponent, VolumeComponent>();
            for (Entity::EntityId volumeEntity : volumeEntities)
            {
                GameCore::TransformComponent& transformComponent = volumeEntities.get<GameCore::TransformComponent>(volumeEntity);
                VolumeComponent& volumeComponent = volumeEntities.get<VolumeComponent>(volumeEntity);

                bool hasPositionChanged = volumeComponent.Chunk->GetPosition() != transformComponent.GetTranslation();
                if (isModified || hasPositionChanged)
                {
                    volumeComponent.IsLoading = true;
                    volumeComponent.Chunk->SetPosition(transformComponent.GetTranslation());
                    volumeComponent.Chunk->SetSize(chunkSize);
                    volumeComponent.Chunk->RemoveAllMeshChangedHandlers();
                    volumeComponent.Chunk->AddMeshChangedHandler([&, entity = volumeEntity](const MeshBuilder& meshBuilder)
                        {
                            VolumeComponent& volume = registry.GetComponent<VolumeComponent>(entity);
                            const GameCore::TransformComponent& transform = registry.GetComponent<GameCore::TransformComponent>(entity);
                            OnVolumeLoaded(api, volume, transform, meshBuilder);
                        });

                    volumeComponent.Chunk->Load(IsoSurfaceMethod::DMC, 32, chunkSize, loc_GeometricError, sampleResolution, loc_GeometricError, maxDistanceSkirts, *volumeSource);
                }
            }

        }

    }
    

    void VolumeRendering::system(onyxU64 /*deltaTime*/, GameCore::Scene& scene, Graphics::GraphicsApi& api, Assets::AssetSystem& assetSytem)
    {
        // MOVE THIS TO GAMECORE
        Graphics::FrameContext& frameContext = api.GetFrameContext();

        if (frameContext.FrameData == nullptr)
            frameContext.FrameData = MakeUnique<GameCore::SceneFrameData>();

        GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);
        sceneFrameData.m_StaticMeshDrawCalls.clear();
        // MOVE THIS TO GAMECORE END

        auto csgEntities = scene.GetRegistry().GetView<GameCore::MaterialComponent, VolumeComponent>();
        for (Entity::EntityId volumeEntity : csgEntities)
        {
            GameCore::MaterialComponent& materialComponent = csgEntities.get<GameCore::MaterialComponent>(volumeEntity);

            if (materialComponent.Material.IsValid() == false)
            {
                // this should be moved to the component create
                materialComponent.LoadMaterial(assetSytem);
                continue;
            }

            if ((materialComponent.Material.IsValid() == false) || materialComponent.Material->IsLoading())
                continue;

            VolumeComponent& volumeComponent = csgEntities.get<VolumeComponent>(volumeEntity);

            if (volumeComponent.IsLoading || (volumeComponent.Vertices.IsValid() == false))
                continue;

            GameCore::StaticMeshDrawCall& drawCall = sceneFrameData.m_StaticMeshDrawCalls.emplace_back();
            drawCall.VertexData = volumeComponent.Vertices;
            drawCall.Indices = volumeComponent.Indices;
            drawCall.Material = materialComponent.Material;
        }

    }

    VolumeComponent::VolumeComponent()
        : Chunk(new VolumeChunk())
    {
    }

    void VolumeComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void VolumeComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void VolumeComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void VolumeComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void VolumeSourceComponent::Serialize(Stream& outStream) const
    {
        ONYX_UNUSED(outStream);
    }

    void VolumeSourceComponent::Deserialize(const Stream& inStream)
    {
        ONYX_UNUSED(inStream);
    }

    void VolumeSourceComponent::SerializeJson(FileSystem::JsonValue& outStream) const
    {
        if (Volume == nullptr)
            return;

        outStream.Set("volumeType", VolumeType);

        constexpr onyxU32 SPHERE_TYPE = TypeHash<CSGSphere>();
        constexpr onyxU32 PLANE_TYPE = TypeHash<CSGPlane>();
        constexpr onyxU32 NOISE_TYPE = TypeHash<SimplexNoiseSource>();

        // can't do a switch because MSVC is not happy with the type hashes (it considers them the same)
        if (VolumeType == SPHERE_TYPE)
        {
            CSGSphere* sphere = static_cast<CSGSphere*>(Volume);
            Array<onyxF32, 3> center { sphere->GetCenter()[0], sphere->GetCenter()[1], sphere->GetCenter()[2] };

            outStream.Set("center", center);
            outStream.Set("radius", sphere->GetRadius());
        } 
        else if (VolumeType == PLANE_TYPE)
        {
            CSGPlane* plane = static_cast<CSGPlane*>(Volume);
            Array<onyxF32, 3> normal { plane->GetNormal()[0], plane->GetNormal()[1], plane->GetNormal()[2] };
            outStream.Set("normal", normal);
            outStream.Set("distance", plane->GetDistance());
        }
        else if (VolumeType == NOISE_TYPE)
        {
            SimplexNoiseSource* noiseSource = static_cast<SimplexNoiseSource*>(Volume);
           
            outStream.Set("dimension", noiseSource->GetDimension());
            outStream.Set("octaves", noiseSource->GetOctaves());
            outStream.Set("amplitude", noiseSource->GetAmplitude());
            outStream.Set("frequency", noiseSource->GetFrequency());
            outStream.Set("lacunarity", noiseSource->GetLacunarity());
            outStream.Set("gain", noiseSource->GetGain());
            outStream.Set("scale", noiseSource->GetScale());
        }
    }

    void VolumeSourceComponent::DeserializeJson(const FileSystem::JsonValue& inStream)
    {
        inStream.Get("volumeType", VolumeType);

        onyxU64 materialId;
        inStream.Get("material", materialId);
        Material = materialId;

        constexpr onyxU32 SPHERE_TYPE = TypeHash<CSGSphere>();
        constexpr onyxU32 PLANE_TYPE = TypeHash<CSGPlane>();
        constexpr onyxU32 NOISE_TYPE = TypeHash<SimplexNoiseSource>();

        // can't do a switch because MSVC is not happy with the type hashes (it considers them the same)
        if (VolumeType == SPHERE_TYPE)
        {
            Array<onyxF32, 3> center;
            inStream.Get("center", center);

            onyxF32 radius;
            inStream.Get("radius", radius);
            Volume = new CSGSphere(radius, { center[0], center[1], center[2] });
        }
        else if (VolumeType == PLANE_TYPE)
        {
            Array<onyxF32, 3> normal;
            inStream.Get("normal", normal);

            onyxF32 distance;
            inStream.Get("distance", distance);
            Volume = new CSGPlane(distance, { normal[0], normal[1], normal[2] });
        }
        else if (VolumeType == NOISE_TYPE)
        {
            SimplexNoiseSource* noiseSource = new SimplexNoiseSource();

            SimplexNoiseSource::Dimension dimension;
            inStream.Get("dimension", dimension, SimplexNoiseSource::Dimension::Dimension_2D);
            noiseSource->SetDimension(dimension);

            onyxU32 octaves;
            inStream.Get("octaves", octaves, 0u);
            noiseSource->SetOctaves(octaves);

            onyxF32 amplitude;
            inStream.Get("amplitude", amplitude, 0.0f);
            noiseSource->SetAmplitude(amplitude);

            onyxF32 frequency;
            inStream.Get("frequency", frequency, 0.0f);
            noiseSource->SetFrequency(frequency);

            onyxF32 lacunarity;
            inStream.Get("lacunarity", lacunarity, 0.0f);
            noiseSource->SetLacunarity(lacunarity);

            onyxF32 gain;
            inStream.Get("gain", gain, 0.0f);
            noiseSource->SetGain(gain);

            onyxF32 scale;
            inStream.Get("scale", scale, 0.0f);
            noiseSource->SetScale(scale);

            Volume = noiseSource;
        }

        IsModified = true;
    }
}
