#pragma once
#include <onyx/engine/enginesystem.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::GameCore
{
    class GameCoreSystem : public IEngineSystem
    {
    public:
        void Init(Assets::AssetSystem& assetSystem, Graphics::GraphicsApi& graphicsApi);

        void Update(uint64_t deltaTime);

        void SetScene(Reference<Scene>& scene) { Scene = scene; }

        auto& GetGameLoop() { return GameLoop; }

    private:
        DynamicArray<InplaceFunction<void(onyxU64, Scene& scene, Graphics::GraphicsApi&, Assets::AssetSystem&)>> GameLoop;
        Reference<Scene> Scene;
    };
}
