#pragma once
#include <onyx/engine/enginesystem.h>

namespace onyx::assets
{
    class AssetSystem;
}

namespace onyx::game_core
{
    class GameCoreSystem;
}

namespace onyx::volume
{
    class VolumeModule : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "onyx::volume::VolumeSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        VolumeModule(game_core::GameCoreSystem& gameCore);
        ~VolumeModule() override;
    };
}
