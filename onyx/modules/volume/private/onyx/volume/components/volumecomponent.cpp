#include <onyx/volume/components/volumecomponent.h>

#include <onyx/entity/entityregistry.h>

#include <onyx/gamecore/components/graphics/materialcomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/scene/sceneframedata.h>

// TODO: This is needed for the dependant function args - we should move them somewhere so they are included already
#include <onyx/gamecore/gamecore.h>
#include <onyx/entity/entitycomponentsystem.h>

#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/vertex.h>

#include <onyx/volume/isosurface/marchingsquaressurface_cms.h>
#include <onyx/volume/source/csg/csgcube.h>
#include <onyx/volume/source/csg/csgplane.h>
#include <onyx/volume/source/csg/csgsphere.h>
#include <onyx/volume/source/noise/simplexnoisesource.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>
#include <onyx/volume/graphics/generatemeshpass.h>

namespace Onyx
{

namespace Volume
{
    namespace VolumeSource
    {
        constexpr onyxF32 loc_GeometricError = 0.8f;

        void OnVolumeLoaded(Graphics::GraphicsApi& api, VolumeComponent& volumeComponent, const MeshBuilder& meshBuilder)
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
                vertices.emplace_back(Vector3f(vertex.Position[0], vertex.Position[1], vertex.Position[2]) + volumeComponent.Chunk->GetPosition(), vertex.Normal, uv);
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

        using VolumeSourceEntitiesQuery = Entity::EntityQuery<VolumeSourceComponent>;
        using VolumeComponentQuery = Entity::EntityQuery<const GameCore::TransformComponent, VolumeComponent>;
        void system(VolumeSourceEntitiesQuery volumeSourceQuery, VolumeComponentQuery volumeEntitiesQuery, Graphics::GraphicsApi& graphicsApi)
        {
            VolumeBase* volumeSource = nullptr;
            auto& csgEntities = volumeSourceQuery.GetView();
            bool isModified = false;
            for (Entity::EntityId volumeEntity : csgEntities)
            {
                auto&& [volumeComponent] = csgEntities.get(volumeEntity);

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

            onyxF32 chunkSize = 128.0f;
            onyxF32 sampleResolution = 1.0f;
            onyxF32 maxDistanceSkirts = 1.0f;

            auto volumeEntities = volumeEntitiesQuery.GetView();
            for (Entity::EntityId volumeEntity : volumeEntities)
            {
                auto&& [ transformComponent, volumeComponent ]  = volumeEntities.get(volumeEntity);

                bool hasPositionChanged = volumeComponent.Chunk->GetPosition() != transformComponent.GetTranslation();
                if (isModified || hasPositionChanged)
                {
                    volumeComponent.IsLoading = true;
                    volumeComponent.Chunk->SetPosition(transformComponent.GetTranslation());
                    volumeComponent.Chunk->SetSize(chunkSize);
                    volumeComponent.Chunk->RemoveAllMeshChangedHandlers();
                    volumeComponent.Chunk->AddMeshChangedHandler([&](const MeshBuilder& meshBuilder)
                    {
                        OnVolumeLoaded(graphicsApi, volumeComponent, meshBuilder);
                    });

                    volumeComponent.Chunk->Load(IsoSurfaceMethod::CMS, 4, chunkSize, loc_GeometricError, sampleResolution, loc_GeometricError, maxDistanceSkirts, *volumeSource);
                }
            }
        }

    }

    namespace VolumeRendering
    {
        // TODO: Material component should be read access
        using VolumeEntityAccess = Entity::Entity<const VolumeComponent, GameCore::MaterialComponent>;
        void system(VolumeEntityAccess entity, Graphics::FrameContext& frameContext, Assets::AssetSystem& assetSytem)
        {
            GameCore::SceneFrameData& sceneFrameData = static_cast<GameCore::SceneFrameData&>(*frameContext.FrameData);
            
            auto&& [ volumeComponent, materialComponent ] = entity.Get();

            if (materialComponent.Material.IsValid() == false)
            {
                // this should be moved to the component create
                materialComponent.LoadMaterial(assetSytem);
                return;
            }

            if ((materialComponent.Material.IsValid() == false) || materialComponent.Material->IsLoading())
                return;
            
            if (volumeComponent.IsLoading || (volumeComponent.Vertices.IsValid() == false))
                return;


            GameCore::StaticMeshIndirectDrawCall& drawCall = sceneFrameData.m_StaticMeshIndirectDrawCalls.emplace_back();
            drawCall.VertexData = GenerateVolumeMesh::GetVertexBuffer();
            drawCall.DrawCommandBuffer = GenerateVolumeMesh::GetDrawCommandBuffer();
            drawCall.Material = materialComponent.Material;
        }
    }

    namespace Systems
    {
        void registerSystem(Entity::EntityComponentSystemsGraph& ecsGraph)
        {
            ecsGraph.Register(VolumeSource::system);
            ecsGraph.Register(VolumeRendering::system);
        }
    }
    
    VolumeComponent::VolumeComponent()
        : Chunk(new VolumeChunk())
    {
    }
}


bool Serialization<Volume::VolumeComponent>::Serialize(Serializer& /*serializer*/, const Volume::VolumeComponent& /*volume*/)
{
    // Nothing to serialize yet
    return true;
}

bool Serialization<Volume::VolumeComponent>::Deserialize(const Deserializer& /*deserializer*/, Volume::VolumeComponent& /*outVolume*/)
{
    // Nothing to serialize yet
    return true;
}

bool Serialization<Volume::VolumeSourceComponent>::Serialize(Serializer& serializer, const Volume::VolumeSourceComponent& volumeSource)
{
    if (volumeSource.Volume == nullptr)
    {
        return true;
    }

    serializer.Write<"volumeType">(volumeSource.VolumeType);

    constexpr StringId32 SPHERE_TYPE("CSGSphere");
    constexpr StringId32 PLANE_TYPE("CSGPlane");
    constexpr StringId32 NOISE_TYPE("SimplexNoiseSource");

    // can't do a switch because MSVC is not happy with the type hashes (it considers them the same)
    if (volumeSource.VolumeType == SPHERE_TYPE)
    {
        Volume::CSGSphere* sphere = static_cast<Volume::CSGSphere*>(volumeSource.Volume);
        serializer.Write<"center">(sphere->GetCenter());
        serializer.Write<"radius">( sphere->GetRadius());
    }
    else if (volumeSource.VolumeType == PLANE_TYPE)
    {
        Volume::CSGPlane* plane = static_cast<Volume::CSGPlane*>(volumeSource.Volume);
        serializer.Write<"normal">(plane->GetNormal());
        serializer.Write<"distance">(plane->GetDistance());
    }
    else if (volumeSource.VolumeType == NOISE_TYPE)
    {
        Volume::SimplexNoiseSource* noiseSource = static_cast<Volume::SimplexNoiseSource*>(volumeSource.Volume);

        serializer.Write<"dimension">(noiseSource->GetDimension());
        serializer.Write<"octaves">(noiseSource->GetOctaves());
        serializer.Write<"amplitude">(noiseSource->GetAmplitude());
        serializer.Write<"frequency">(noiseSource->GetFrequency());
        serializer.Write<"lacunarity">(noiseSource->GetLacunarity());
        serializer.Write<"gain">(noiseSource->GetGain());
        serializer.Write<"scale">(noiseSource->GetScale());
    }

    return true;
}
bool Serialization<Volume::VolumeSourceComponent>::Deserialize(const Deserializer& deserializer, Volume::VolumeSourceComponent& outVolumeSource)
{
    deserializer.Read<"volumeType">(outVolumeSource.VolumeType);

    onyxU64 materialId;
    deserializer.Read<"material">(materialId);
    outVolumeSource.Material = materialId;

    constexpr StringId32 SPHERE_TYPE("CSGSphere");
    constexpr StringId32 PLANE_TYPE("CSGPlane");
    constexpr StringId32 NOISE_TYPE("SimplexNoiseSource");

    // can't do a switch because MSVC is not happy with the type hashes (it considers them the same)
    if (outVolumeSource.VolumeType == SPHERE_TYPE)
    {
        Vector3f center;
        deserializer.Read<"center">(center);

        onyxF32 radius;
        deserializer.Read<"radius">(radius);
        outVolumeSource.Volume = new Volume::CSGSphere(radius, center);
    }
    else if (outVolumeSource.VolumeType == PLANE_TYPE)
    {
        Vector3f normal;
        deserializer.Read<"normal">(normal);

        onyxF32 distance;
        deserializer.Read<"distance">(distance);
        outVolumeSource.Volume = new Volume::CSGPlane(distance, normal);
    }
    else if (outVolumeSource.VolumeType == NOISE_TYPE)
    {
        Volume::SimplexNoiseSource* noiseSource = new Volume::SimplexNoiseSource();

        Volume::SimplexNoiseSource::Dimension dimension = Volume::SimplexNoiseSource::Dimension::Dimension_2D;
        deserializer.Read<"dimension">(dimension);
        noiseSource->SetDimension(dimension);

        onyxU32 octaves = 0;
        deserializer.Read<"octaves">(octaves);
        noiseSource->SetOctaves(octaves);

        onyxF32 amplitude = 0.0f;
        deserializer.Read<"amplitude">(amplitude);
        noiseSource->SetAmplitude(amplitude);

        onyxF32 frequency = 0.0f;
        deserializer.Read<"frequency">(frequency);
        noiseSource->SetFrequency(frequency);

        onyxF32 lacunarity = 0.0f;
        deserializer.Read<"lacunarity">(lacunarity);
        noiseSource->SetLacunarity(lacunarity);

        onyxF32 gain = 0.0f;
        deserializer.Read<"gain">(gain);
        noiseSource->SetGain(gain);

        onyxF32 scale = 0.0f;
        deserializer.Read<"scale">(scale);
        noiseSource->SetScale(scale);

        outVolumeSource.Volume = noiseSource;
    }

    outVolumeSource.IsModified = true;
    return true;
}

}


