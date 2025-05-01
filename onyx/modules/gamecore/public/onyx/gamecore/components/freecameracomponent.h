#pragma once
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
    class Scene;

    namespace FreeCamera
    {
        void system(onyxU64 deltaTime, Scene& scene, Graphics::GraphicsApi&, Assets::AssetSystem&);
    }

    struct FreeCameraControllerComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::FreeCameraControllerComponent";
        StringId32 GetTypeId() const { return TypeId; }

        onyxF32 BaseVelocity = 0.002f;
        onyxF32 MinVelocity = 0.001f;
        onyxF32 MaxVelocity = 0.3f;

        onyxF32 RotationVelocity = 0.3f;

        onyxF32 VelocityIncrementFactor = 0.3f;

        void Serialize(Stream& outStream) const;
        void Deserialize(const Stream& inStream);

        void SerializeJson(FileSystem::JsonValue& outStream) const;
        void DeserializeJson(const FileSystem::JsonValue& inStream);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        void DrawImGuiEditor();
#endif
    };

    struct FreeCameraRuntimeComponent
    {
        static constexpr bool HideInEditor = true;
        static constexpr bool IsTransient = true;

        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::FreeCameraRuntimeComponent";
        StringId32 GetTypeId() const { return TypeId; }

        Vector3f InputDirection;
        Vector3f InputRotation;

        onyxF32 Velocity = 0.0f;

        // rotation around X
        onyxF32 Pitch = 0.0f;
        // rotation around Y
        onyxF32 Yaw = 0.0f;

        onyxF32 PitchDelta = 0.0f;
        onyxF32 YawDelta = 0.0f;
    };
}
