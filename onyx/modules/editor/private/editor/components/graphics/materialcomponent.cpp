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

#endif
}
