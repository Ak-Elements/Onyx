#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    void TextComponent::DrawImGuiEditor()
    {
        if (Ui::PropertyGrid::DrawStringProperty("Text", Text))
        {
            UpdateVertexBuffers();
        }

        Ui::PropertyGrid::DrawScalarProperty("Size", Size);
    }

#endif  
}
