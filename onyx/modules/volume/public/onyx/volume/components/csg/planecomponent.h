#pragma once

namespace Onyx::Volume
{
    struct PlaneComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::PlaneComponent";
        StringId32 GetTypeId() const { return TypeId; }
    };
}
