#pragma once

namespace Onyx
{
    class Stream;

    namespace FileSystem
    {
        struct JsonValue;
    }
}

namespace Onyx
{

namespace GameCore
{   
    struct IdComponent
    {
        static constexpr StringId32 TypeId = "Onyx::GameCore::Components::IdComponent";
        StringId32 GetTypeId() const { return TypeId; }

        static constexpr bool HideInEditor = true;

        //TODO: change to UUID
        onyxU64 Id;

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked in onyx_entity 
        bool DrawProperties(bool showHidden);
#endif
    };
}

template <>
struct Serialization<GameCore::IdComponent>
{
    static bool Serialize(Serializer& serializer, const GameCore::IdComponent& id);
    static bool Deserialize(const Deserializer& deserializer, GameCore::IdComponent& outId);
};

}
