#include <onyx/nodegraph/pins/pinmeta.hpp>
#include <onyx/graphics/graphicshandles.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

#include <onyx/ui/propertygrid.h>

namespace Onyx::NodeGraph
{
    void PinMetaObject<ExecutePin>::Register()
    {
        NodeGraphTypeRegistry::Register<ExecutePin, "execute">();
    }

    void PinMetaObject<bool>::Register()
    {
        NodeGraphTypeRegistry::Register<bool, "bool">();
    }

    bool PinMetaObject<bool>::DrawPinInPropertyGrid(StringView name, bool& value)
    {
        return Ui::PropertyGrid::DrawBoolProperty(name, value);
    }

    void PinMetaObject<onyxS8>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxS8, "onyxS8">();
    }

    bool PinMetaObject<onyxS8>::DrawPinInPropertyGrid(StringView name, onyxS8& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxS16>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxS16, "onyxS16">();
    }

    bool PinMetaObject<onyxS16>::DrawPinInPropertyGrid(StringView name, onyxS16& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxS32>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxS32, "onyxS32">();
    }

    bool PinMetaObject<onyxS32>::DrawPinInPropertyGrid(StringView name, onyxS32& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxS64>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxS64, "onyxS64">();
    }
    
    bool PinMetaObject<onyxS64>::DrawPinInPropertyGrid(StringView name, onyxS64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxU8>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxU8, "onyxU8">();
    }

    bool PinMetaObject<onyxU8>::DrawPinInPropertyGrid(StringView name, onyxU8& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxU16>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxU16, "onyxU8">();
    }
    
    bool PinMetaObject<onyxU16>::DrawPinInPropertyGrid(StringView name, onyxU16& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxU32>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxU32, "onyxU32">();
    }

    bool PinMetaObject<onyxU32>::DrawPinInPropertyGrid(StringView name, onyxU32& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxU64>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxU64, "onyxU64">();
    }
    
    bool PinMetaObject<onyxU64>::DrawPinInPropertyGrid(StringView name, onyxU64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxF32>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxF32, "onyxF32">();
    }
    
    bool PinMetaObject<onyxF32>::DrawPinInPropertyGrid(StringView name, onyxF32& value)
    {

        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<onyxF64>::Register()
    {
        NodeGraphTypeRegistry::Register<onyxF64, "onyxF64">();
    }
    
    bool PinMetaObject<onyxF64>::DrawPinInPropertyGrid(StringView name, onyxF64& value)
    {
        return Ui::PropertyGrid::DrawScalarProperty(name, value);
    }

    void PinMetaObject<Vector2s32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector2s32, "vector2s32">();
    }

    bool PinMetaObject<Vector2s32>::DrawPinInPropertyGrid(StringView name, Vector2s32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector2s64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector2s64, "vector2s64">();
    }

    bool PinMetaObject<Vector2s64>::DrawPinInPropertyGrid(StringView name, Vector2s64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector2f32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector2f32, "vector2f32">();
    }
    
    bool PinMetaObject<Vector2f32>::DrawPinInPropertyGrid(StringView name, Vector2f32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector2f64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector2f64, "vector2f64">();
    }

    bool PinMetaObject<Vector2f64>::DrawPinInPropertyGrid(StringView name, Vector2f64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector3s32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector3s32, "vector3s32">();
    }
    
    bool PinMetaObject<Vector3s32>::DrawPinInPropertyGrid(StringView name, Vector3s32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector3s64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector3s64, "vector3s64">();
    }
    
    bool PinMetaObject<Vector3s64>::DrawPinInPropertyGrid(StringView name, Vector3s64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector3f32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector3f32, "vector3f32">();
    }
    
    bool PinMetaObject<Vector3f32>::DrawPinInPropertyGrid(StringView name, Vector3f32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector3f64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector3f64, "vector3f64">();
    }
    
    bool PinMetaObject<Vector3f64>::DrawPinInPropertyGrid(StringView name, Vector3f64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector4s32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector4s32, "vector4s32">();
    }
    
    bool PinMetaObject<Vector4s32>::DrawPinInPropertyGrid(StringView name, Vector4s32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector4s64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector4s64, "vector4s64">();
    }

    bool PinMetaObject<Vector4s64>::DrawPinInPropertyGrid(StringView name, Vector4s64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector4f32>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector4f32, "vector4f32">();
    }

    bool PinMetaObject<Vector4f32>::DrawPinInPropertyGrid(StringView name, Vector4f32& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<Vector4f64>::Register()
    {
        NodeGraphTypeRegistry::Register<Vector4f64, "vector4f64">();
    }
    
    bool PinMetaObject<Vector4f64>::DrawPinInPropertyGrid(StringView name, Vector4f64& value)
    {
        return Ui::PropertyGrid::DrawVectorProperty(name, value);
    }

    void PinMetaObject<String>::Register()
    {
        NodeGraphTypeRegistry::Register<String, "string">();
    }

    bool PinMetaObject<String>::DrawPinInPropertyGrid(StringView name, String& value)
    {
        return Ui::PropertyGrid::DrawStringProperty(name, value);
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
