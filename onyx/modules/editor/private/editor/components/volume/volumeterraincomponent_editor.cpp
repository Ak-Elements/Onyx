#include <onyx/volume/components/volumeterraincomponent.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::Volume
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool VolumeTerrainSettingsComponent::DrawImGuiEditor()
    {
        constexpr StringView DimensionsTooltip = "The dimension limits of the terrain in each axis \n if e.g. x is set to 10 and chunk size is 32, the grid ranges max from -16.000 to 16.000 \n if e.g. x is set to -1 it can infinitely scale on the x axis";
            
        bool isModified = Ui::PropertyGrid::DrawVector3Property("Dimensions", DimensionsTooltip,Dimensions, { -1, -1, -1 } );

        if (Ui::PropertyGrid::BeginPropertyGroup("Chunk"))
        {
            isModified |= Ui::PropertyGrid::DrawScalarProperty("Size", ChunkSize, Ui::ScalarInputFlag::PowerOf2);
            isModified |= Ui::PropertyGrid::DrawScalarProperty("Resolution", Resolution, Ui::ScalarInputFlag::PowerOf2);
            Ui::PropertyGrid::EndPropertyGroup();
        }

        return isModified;
    }
#endif
}
