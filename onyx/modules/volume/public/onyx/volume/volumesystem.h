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
        void Init(GameCore::GameCoreSystem& gameCore);
    };
}
