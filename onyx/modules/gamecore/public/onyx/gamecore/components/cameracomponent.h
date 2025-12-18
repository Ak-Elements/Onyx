#pragma once
#include <onyx/entity/entitycomponentsystem.h>

namespace Onyx
{

namespace GameCore
{
    struct CameraComponent;
    struct TransformComponent;

    namespace Camera
    {
        void system(Entity::EntityQuery<const TransformComponent, CameraComponent> query);
    }

    //struct CameraComponent
    //{
    //    static constexpr bool HideInEditor = true;
    //
    //    static constexpr StringId32 TypeId = "Onyx::GameCore::Components::CameraComponent";
    //    StringId32 GetTypeId() const { return TypeId; }
    //
    //    bool IsPrimary;
    //    Graphics::Camera Camera;
    //};
}

//template <>
//struct Serialization<GameCore::CameraComponent>
//{
//    static bool Serialize(Serializer& serializer, const GameCore::CameraComponent& camera);
//    static bool Deserialize(const Deserializer& deserializer, GameCore::CameraComponent& outCamera);
//};

}
