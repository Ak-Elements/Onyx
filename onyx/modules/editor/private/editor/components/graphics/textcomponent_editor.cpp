#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool TextComponent::DrawImGuiEditor()
    {
        bool hasModified = Ui::PropertyGrid::DrawStringProperty("Text", Text);
        if (hasModified)
        {
            UpdateVertexBuffers();
        }

        hasModified |= Ui::PropertyGrid::DrawScalarProperty("Size", Size);
        return hasModified;
    }

#endif  
}
