#pragma once

namespace Onyx::Volume
{
    struct SphereComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::SphereComponent";
        StringId32 GetTypeId() const { return TypeId; }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        // this is implemented in the editor module as we do not have ImGui linked directly
        bool DrawImGuiEditor();
#endif

        onyxF32 Radius = 1.0f;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Volume::SphereComponent>
    {
        static bool Serialize(Serializer& serializer, const Volume::SphereComponent& sphereComponent);
        static bool Deserialize(const Deserializer& deserializer, Volume::SphereComponent& outSphereComponent);
    };
}
