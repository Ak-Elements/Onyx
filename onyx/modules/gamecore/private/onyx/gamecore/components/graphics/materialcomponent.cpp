#include <onyx/gamecore/components/graphics/materialcomponent.h>

#include <onyx/assets/assetsystem.h>
#include <onyx/graphics/shadergraph/materialshadergraph.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
namespace GameCore
{
    void MaterialComponent::LoadMaterial(Assets::AssetSystem& assetSystem)
    {
        if (MaterialId.IsValid() == false)
            return;

        assetSystem.GetAsset(MaterialId, Material);
    }
}

bool Serialization<GameCore::MaterialComponent>::Serialize(Serializer& serializer, const GameCore::MaterialComponent& material)
{
    return serializer.Write<"material">(material.MaterialId.Get());
}

bool Serialization<GameCore::MaterialComponent>::Deserialize(const Deserializer& deserializer, GameCore::MaterialComponent& outMaterial)
{
    onyxU64 materialId;
    if (deserializer.Read<"material">(materialId))
    {
        outMaterial.MaterialId = materialId;
    }

    return true;
}


}
