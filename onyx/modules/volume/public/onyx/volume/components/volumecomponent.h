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
        class Scene;
    }

    namespace FileSystem
    {
        struct JsonValue;
    }

    class Stream;
    
}

namespace Onyx::Volume
{
    class VolumeBase;

    namespace VolumeSource
    {
        void system(onyxU64, GameCore::Scene&, Graphics::GraphicsApi& api, Assets::AssetSystem& assetSytem);
    }

    namespace VolumeRendering
    {
        void system(onyxU64, GameCore::Scene&, Graphics::GraphicsApi& api, Assets::AssetSystem& assetSytem);
    }

    struct VolumeComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::VolumeComponent";
        StringId32 GetTypeId() const { return TypeId; }

        VolumeComponent();

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

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


        bool IsModified = false;
        //TODO This should probably not allocate heap memory
        StringId32 VolumeType = 0;
        VolumeBase* Volume = nullptr;

        Assets::AssetId Material;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };
}
