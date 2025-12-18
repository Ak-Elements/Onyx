#include <onyx/gamecore/components/graphics/textcomponent.h>
#include <onyx/ui/propertygrid.h>

namespace Onyx::GameCore
{
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    bool TextComponent::DrawProperties(bool /*showHidden*/)
    {
        bool hasModified = Ui::PropertyGrid::DrawProperty("Text", Text);
        if (hasModified)
        {
            UpdateVertexBuffers();
        }

        hasModified |= Ui::PropertyGrid::DrawProperty("Size", Size);
        return hasModified;
    }

#endif  
}
