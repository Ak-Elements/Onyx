#include <onyx/ui/propertygrid.h>
#include <onyx/graphics/rendergraph/rendergraph.h>
#include <onyx/graphics/graphicstypes.h>

namespace Onyx::NodeGraph
{
    bool PinMetaObject<bool>::DrawPinInPropertyGrid(StringView name, bool& value)
    {
        return Ui::PropertyGrid::DrawBoolProperty(name, value);
    }

    bool PinMetaObject<onyxS8>::DrawPinInPropertyGrid(StringView name, onyxS8& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    bool PinMetaObject<onyxS16>::DrawPinInPropertyGrid(StringView name, onyxS16& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    bool PinMetaObject<onyxS32>::DrawPinInPropertyGrid(StringView name, onyxS32& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxS64>::DrawPinInPropertyGrid(StringView name, onyxS64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxU8>::DrawPinInPropertyGrid(StringView name, onyxU8& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxU16>::DrawPinInPropertyGrid(StringView name, onyxU16& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxU32>::DrawPinInPropertyGrid(StringView name, onyxU32& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxU64>::DrawPinInPropertyGrid(StringView name, onyxU64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxF32>::DrawPinInPropertyGrid(StringView name, onyxF32& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<onyxF64>::DrawPinInPropertyGrid(StringView name, onyxF64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }
    
    bool PinMetaObject<Vector2s32>::DrawPinInPropertyGrid(StringView name, Vector2s32& value)
    {
        return Ui::PropertyGrid::DrawVector2Property(name, value);
    }
    
    bool PinMetaObject<Vector2s64>::DrawPinInPropertyGrid(StringView name, Vector2s64& value)
    {
        return Ui::PropertyGrid::DrawVector2Property(name, value);
    }
    
    bool PinMetaObject<Vector2f>::DrawPinInPropertyGrid(StringView name, Vector2f& value)
    {
        return Ui::PropertyGrid::DrawVector2Property(name, value);
    }
    
    bool PinMetaObject<Vector2d>::DrawPinInPropertyGrid(StringView name, Vector2d& value)
    {
        return Ui::PropertyGrid::DrawVector2Property(name, value);
    }
    
    bool PinMetaObject<Vector3s32>::DrawPinInPropertyGrid(StringView name, Vector3s32& value)
    {
        return Ui::PropertyGrid::DrawVector3Property(name, value);
    }
    
    bool PinMetaObject<Vector3s64>::DrawPinInPropertyGrid(StringView name, Vector3s64& value)
    {
        return Ui::PropertyGrid::DrawVector3Property(name, value);
    }
    
    bool PinMetaObject<Vector3f>::DrawPinInPropertyGrid(StringView name, Vector3f& value)
    {
        return Ui::PropertyGrid::DrawVector3Property(name, value);
    }
    
    bool PinMetaObject<Vector3d>::DrawPinInPropertyGrid(StringView name, Vector3d& value)
    {
        return Ui::PropertyGrid::DrawVector3Property(name, value);
    }
    
    bool PinMetaObject<Vector4s32>::DrawPinInPropertyGrid(StringView name, Vector4s32& value)
    {
        return Ui::PropertyGrid::DrawVector4Property(name, value);
    }
    
    bool PinMetaObject<Vector4s64>::DrawPinInPropertyGrid(StringView name, Vector4s64& value)
    {
        return Ui::PropertyGrid::DrawVector4Property(name, value);
    }
    
    bool PinMetaObject<Vector4f>::DrawPinInPropertyGrid(StringView name, Vector4f& value)
    {
        return Ui::PropertyGrid::DrawVector4Property(name, value);
    }
    
    bool PinMetaObject<Vector4d>::DrawPinInPropertyGrid(StringView name, Vector4d& value)
    {
        return Ui::PropertyGrid::DrawVector4Property(name, value);
    }

    bool PinMetaObject<Graphics::TextureHandle>::DrawPinInPropertyGrid(StringView name, Graphics::TextureHandle& /*value*/)
    {
        String test = "I'm a texture handle";
        return Ui::PropertyGrid::DrawStringProperty(name, test);
    }

    bool PinMetaObject<Graphics::BufferHandle>::DrawPinInPropertyGrid(StringView name, Graphics::BufferHandle& /*value*/)
    {
        String test = "I'm a buffer handle";
        return Ui::PropertyGrid::DrawStringProperty(name, test);
    }
}
