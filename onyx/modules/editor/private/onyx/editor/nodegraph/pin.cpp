#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/pins/pinmeta.hpp>
#include <onyx/rhi/graphicshandles.h>

#include <onyx/ui/propertygrid.h>

namespace onyx::node_graph {
void PinMetaObject< ExecutePin >::Register() {
    NodeGraphTypeRegistry::Register< ExecutePin, "execute" >();
}

void PinMetaObject< bool >::Register() {
    NodeGraphTypeRegistry::Register< bool, "bool" >();
}

bool PinMetaObject< bool >::DrawPinInPropertyGrid( StringView name, bool& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int8_t >::Register() {
    NodeGraphTypeRegistry::Register< int8_t, "int8_t" >();
}

bool PinMetaObject< int8_t >::DrawPinInPropertyGrid( StringView name, int8_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int16_t >::Register() {
    NodeGraphTypeRegistry::Register< int16_t, "int16_t" >();
}

bool PinMetaObject< int16_t >::DrawPinInPropertyGrid( StringView name, int16_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int32_t >::Register() {
    NodeGraphTypeRegistry::Register< int32_t, "int32_t" >();
}

bool PinMetaObject< int32_t >::DrawPinInPropertyGrid( StringView name, int32_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int64_t >::Register() {
    NodeGraphTypeRegistry::Register< int64_t, "int64_t" >();
}

bool PinMetaObject< int64_t >::DrawPinInPropertyGrid( StringView name, int64_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint8_t >::Register() {
    NodeGraphTypeRegistry::Register< uint8_t, "uint8_t" >();
}

bool PinMetaObject< uint8_t >::DrawPinInPropertyGrid( StringView name, uint8_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint16_t >::Register() {
    NodeGraphTypeRegistry::Register< uint16_t, "uint8_t" >();
}

bool PinMetaObject< uint16_t >::DrawPinInPropertyGrid( StringView name, uint16_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint32_t >::Register() {
    NodeGraphTypeRegistry::Register< uint32_t, "uint32_t" >();
}

bool PinMetaObject< uint32_t >::DrawPinInPropertyGrid( StringView name, uint32_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint64_t >::Register() {
    NodeGraphTypeRegistry::Register< uint64_t, "uint64_t" >();
}

bool PinMetaObject< uint64_t >::DrawPinInPropertyGrid( StringView name, uint64_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< float32 >::Register() {
    NodeGraphTypeRegistry::Register< float32, "float32" >();
}

bool PinMetaObject< float32 >::DrawPinInPropertyGrid( StringView name, float32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< float64 >::Register() {
    NodeGraphTypeRegistry::Register< float64, "float64" >();
}

bool PinMetaObject< float64 >::DrawPinInPropertyGrid( StringView name, float64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2s32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector2s32, "vector2s32" >();
}

bool PinMetaObject< Vector2s32 >::DrawPinInPropertyGrid( StringView name, Vector2s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2s64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector2s64, "vector2s64" >();
}

bool PinMetaObject< Vector2s64 >::DrawPinInPropertyGrid( StringView name, Vector2s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2f32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector2f32, "vector2f32" >();
}

bool PinMetaObject< Vector2f32 >::DrawPinInPropertyGrid( StringView name, Vector2f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2f64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector2f64, "vector2f64" >();
}

bool PinMetaObject< Vector2f64 >::DrawPinInPropertyGrid( StringView name, Vector2f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3s32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector3s32, "vector3s32" >();
}

bool PinMetaObject< Vector3s32 >::DrawPinInPropertyGrid( StringView name, Vector3s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3s64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector3s64, "vector3s64" >();
}

bool PinMetaObject< Vector3s64 >::DrawPinInPropertyGrid( StringView name, Vector3s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3f32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector3f32, "vector3f32" >();
}

bool PinMetaObject< Vector3f32 >::DrawPinInPropertyGrid( StringView name, Vector3f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3f64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector3f64, "vector3f64" >();
}

bool PinMetaObject< Vector3f64 >::DrawPinInPropertyGrid( StringView name, Vector3f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4s32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector4s32, "vector4s32" >();
}

bool PinMetaObject< Vector4s32 >::DrawPinInPropertyGrid( StringView name, Vector4s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4s64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector4s64, "vector4s64" >();
}

bool PinMetaObject< Vector4s64 >::DrawPinInPropertyGrid( StringView name, Vector4s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4f32 >::Register() {
    NodeGraphTypeRegistry::Register< Vector4f32, "vector4f32" >();
}

bool PinMetaObject< Vector4f32 >::DrawPinInPropertyGrid( StringView name, Vector4f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4f64 >::Register() {
    NodeGraphTypeRegistry::Register< Vector4f64, "vector4f64" >();
}

bool PinMetaObject< Vector4f64 >::DrawPinInPropertyGrid( StringView name, Vector4f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< String >::Register() {
    NodeGraphTypeRegistry::Register< String, "string" >();
}

bool PinMetaObject< String >::DrawPinInPropertyGrid( StringView name, String& value ) {
    return ui::property_grid::drawProperty( name, value );
}

bool PinMetaObject< rhi::TextureHandle >::DrawPinInPropertyGrid( StringView name, rhi::TextureHandle& /*value*/ ) {
    String test = "I'm a texture handle";
    return ui::property_grid::drawProperty( name, test );
}

bool PinMetaObject< rhi::BufferHandle >::DrawPinInPropertyGrid( StringView name, rhi::BufferHandle& /*value*/ ) {
    String test = "I'm a buffer handle";
    return ui::property_grid::drawProperty( name, test );
}
} // namespace onyx::node_graph
