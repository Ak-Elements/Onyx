#include <onyx/volume/volumesystem.h>
#include <onyx/assets/assetsystem.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/volume/components/volumecomponent.h>

namespace Onyx::Volume
{
    namespace
    {
        void RegisterSystems(DynamicArray<InplaceFunction<void(onyxU64, GameCore::Scene&, Graphics::GraphicsApi&, Assets::AssetSystem&)>>& gameLoop)
        {
            gameLoop.emplace_back(VolumeSource::system);
            gameLoop.emplace_back(VolumeRendering::system);
        }
    }

    void VolumeSystem::Init(GameCore::GameCoreSystem& gameCore)
    {
        RegisterSystems(gameCore.GetGameLoop());

        Entity::EntityRegistry::RegisterComponent<VolumeComponent>();
        Entity::EntityRegistry::RegisterComponent<VolumeSourceComponent>();
    }
}
