#pragma once
#include <onyx/engine/enginesystem.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::GameCore
{
    class GameCoreSystem;
}

namespace Onyx::Volume
{
    class VolumeModule : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Volume::VolumeModule";
        StringId32 GetTypeId() const override { return TypeId; }

        VolumeModule();

        void Init(Assets::AssetSystem& assetSystem, GameCore::GameCoreSystem& gameCore);
        void Shutdown();
    };
}
