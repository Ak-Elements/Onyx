#pragma once
#include <onyx/filesystem/onyxfile.h>

#if !ONYX_IS_RETAIL || ONYX_IS_EDITOR

namespace Onyx
{

namespace GameCore
{
    struct NameComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::NameComponent";
        StringId32 GetTypeId() const { return TypeId; }

        static constexpr bool HideInEditor = true;

        String Name;
    };
}

template <>
struct Serialization<GameCore::NameComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::NameComponent& name);
    static bool Deserialize(const Deserializer& deserializer, GameCore::NameComponent& outName);
};

}
#endif