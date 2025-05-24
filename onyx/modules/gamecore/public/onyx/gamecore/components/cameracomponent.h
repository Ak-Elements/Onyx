#pragma once
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/graphicsapi.h>
#include <onyx/graphics/camera.h>

#include <onyx/entity/entitycomponentsystem.h>

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
    struct CameraComponent;
    struct TransformComponent;

    namespace Camera
    {
        void system(Entity::EntityQuery<const TransformComponent, CameraComponent> query);
    }

    struct CameraComponent
    {
        static constexpr bool HideInEditor = true;

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
