#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/ui/propertygrid.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

namespace Onyx::Volume
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool TerrainSettingsComponent::DrawProperties(bool /*showHidden*/)
    {
        bool isModified = false;

        isModified |= Ui::PropertyGrid::DrawProperty("Size", Size, Ui::ScalarInputFlag::PowerOf2);

        if (Ui::PropertyGrid::BeginPropertyGroup("Chunk"))
        {
            isModified |= Ui::PropertyGrid::DrawProperty("Size", ChunkSize, Ui::ScalarInputFlag::PowerOf2);
            isModified |= Ui::PropertyGrid::DrawProperty("Resolution", Resolution, Ui::ScalarInputFlag::PowerOf2);
            Ui::PropertyGrid::EndPropertyGroup();
        }

        isModified |= Ui::PropertyGrid::DrawAssetSelector("Volume Graph", VolumeGraphAssetId, static_cast<Assets::AssetType>(VolumeShaderGraph::TypeId.GetId()));
        return isModified;
    }
#endif
}