#pragma once
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/camera.h>

//#include <onyx/graphics/camera.h>

namespace Onyx
{
    namespace Assets
    {
        class AssetSystem;
    }

    namespace Graphics
    {
        class GraphicsApi;
    }

    namespace FileSystem
    {
        struct JsonValue;
    }

    class Stream;
}

namespace Onyx::GameCore
{
    namespace Camera
    {
        void system(onyxU64 deltaTime, Scene& scene, Graphics::GraphicsApi&, Assets::AssetSystem&);
    }

    struct CameraComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::CameraComponent";
        StringId32 GetTypeId() const { return TypeId; }

        bool IsPrimary;
        Graphics::Camera Camera;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);
    };
}
