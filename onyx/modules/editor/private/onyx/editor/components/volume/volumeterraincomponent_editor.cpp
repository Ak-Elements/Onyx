#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/ui/propertygrid.h>
#include <onyx/volume/shadergraph/volumeshadergraph.h>

namespace Onyx::Entity
{
    bool ComponentInspector<Volume::TerrainSettingsComponent>::Draw(Volume::TerrainSettingsComponent& component, bool /*showHidden*/)
    {
        bool isModified = false;

        isModified |= Ui::PropertyGrid::DrawProperty("Size", component.Size, Ui::ScalarInputFlag::PowerOf2);

        if (Ui::PropertyGrid::BeginPropertyGroup("Chunk"))
        {
            isModified |= Ui::PropertyGrid::DrawProperty("Size", component.ChunkSize, Ui::ScalarInputFlag::PowerOf2);
            isModified |= Ui::PropertyGrid::DrawProperty("Resolution", component.Resolution, Ui::ScalarInputFlag::PowerOf2);
            Ui::PropertyGrid::EndPropertyGroup();
        }

        isModified |= Ui::PropertyGrid::DrawAssetSelector("Volume Graph", component.VolumeGraphAssetId, static_cast<Assets::AssetType>(Volume::VolumeShaderGraph::TypeId.GetId()));
        return isModified;
    }
}