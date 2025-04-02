#include <onyx/gamecore/components/graphics/materialcomponent.h>

#include <onyx/ui/propertygrid.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR

    void MaterialComponent::DrawImGuiEditor()
    {
        if (Ui::PropertyGrid::DrawAssetSelector("Material", MaterialId, static_cast<Assets::AssetType>(TypeHash<Graphics::MaterialShaderGraph>())))
        {
            Material.Reset();
        }
    }

    void MaterialComponent::LoadMaterial(Assets::AssetSystem& assetSystem)
    {
        if (MaterialId.IsValid() == false)
            return;

        assetSystem.GetAsset(MaterialId, Material);
    }

#endif
}
