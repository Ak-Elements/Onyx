#pragma once
#include <onyx/assets/asset.h>
#include <onyx/graphics/graphicshandles.h>
#include <onyx/volume/chunk/volumechunk.h>

namespace Onyx
{
    namespace Assets
    {
        class AssetSystem;
    }

    namespace GameCore
    {
        struct TransformComponent;
        struct MaterialComponent;
        class Scene;
    }

    namespace Entity
    {
        class EntityComponentSystemsGraph;
    }
}

namespace Onyx
{

namespace Volume
{
    struct VolumeSourceComponent;
    struct VolumeComponent;
    class VolumeBase;

    namespace Systems
    {
        void registerSystem(Entity::EntityComponentSystemsGraph& ecsGraph);
    }

    struct VolumeComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::VolumeComponent";
        StringId32 GetTypeId() const { return TypeId; }

        VolumeComponent();

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif

        VolumeChunk* Chunk;

        Graphics::BufferHandle Vertices;
        Graphics::BufferHandle Indices;

        bool IsLoading = false;
    };

    struct VolumeSourceComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::VolumeSourceComponent";
        StringId32 GetTypeId() const { return TypeId; }

        //TODO This should probably not allocate heap memory
        StringId32 VolumeType = 0;
        VolumeBase* Volume = nullptr;

        Assets::AssetId Material;
        bool IsModified = false;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };
}

template <>
struct Serialization<Volume::VolumeComponent>
{
    static bool Serialize(Serializer& serializer, const Volume::VolumeComponent& volume);
    static bool Deserialize(const Deserializer& deserializer, Volume::VolumeComponent& outVolume);
};

template <>
struct Serialization<Volume::VolumeSourceComponent>
{
    static bool Serialize(Serializer& serializer, const Volume::VolumeSourceComponent& volumeSource);
    static bool Deserialize(const Deserializer& deserializer, Volume::VolumeSourceComponent& outVolumeSource);
};

}
