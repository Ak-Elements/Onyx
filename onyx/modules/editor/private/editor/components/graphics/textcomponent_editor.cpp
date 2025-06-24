#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool TextComponent::DrawImGuiEditor()
    {
        if (Ui::PropertyGrid::DrawStringProperty("Text", Text))
        {
            UpdateVertexBuffers();
            return true;
        }

        return Ui::PropertyGrid::DrawScalarProperty("Size", Size);
    }

#endif  
}
