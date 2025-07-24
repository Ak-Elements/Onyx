#pragma once
#include <onyx/engine/enginesystem.h>

namespace Onyx::GameCore
{
    class GameCoreSystem;
}

namespace Onyx::Volume
{
    class VolumeSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeModule";
        StringId32 GetTypeId() const { return TypeId; }

        VolumeSystem();

        void Init(GameCore::GameCoreSystem& gameCore);
    };
}
