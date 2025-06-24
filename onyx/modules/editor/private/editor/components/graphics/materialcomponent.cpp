#include <onyx/gamecore/components/graphics/materialcomponent.h>

#include <onyx/ui/propertygrid.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR

    bool MaterialComponent::DrawImGuiEditor()
    {
        if (Ui::PropertyGrid::DrawAssetSelector("Material", MaterialId, static_cast<Assets::AssetType>(Graphics::MaterialShaderGraph::TypeId.GetId())))
        {
            Material.Reset();
            return true;
        }

        return false;
    }

#endif
}