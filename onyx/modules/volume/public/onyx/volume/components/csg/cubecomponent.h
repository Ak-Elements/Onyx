#pragma once

namespace Onyx::Volume
{
    struct CubeComponent
    {
        static constexpr StringId32 TypeId = "Onyx::Volume::Components::CubeComponent";
        StringId32 GetTypeId() const { return TypeId; }
    };
}