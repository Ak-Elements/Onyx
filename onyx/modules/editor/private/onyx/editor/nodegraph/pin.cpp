#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/pins/pinmeta.hpp>
#include <onyx/rhi/graphicshandles.h>

#include <onyx/ui/propertygrid/propertygrid.h>

namespace onyx::node_graph {
void PinMetaObject< ExecutePin >::registerType() {
    NodeGraphTypeRegistry::registerType< ExecutePin, "execute" >();
}

void PinMetaObject< bool >::registerType() {
    NodeGraphTypeRegistry::registerType< bool, "bool" >();
}

bool PinMetaObject< bool >::drawPinInPropertyGrid( StringView name, bool& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int8_t >::registerType() {
    NodeGraphTypeRegistry::registerType< int8_t, "int8_t" >();
}

bool PinMetaObject< int8_t >::drawPinInPropertyGrid( StringView name, int8_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int16_t >::registerType() {
    NodeGraphTypeRegistry::registerType< int16_t, "int16_t" >();
}

bool PinMetaObject< int16_t >::drawPinInPropertyGrid( StringView name, int16_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int32_t >::registerType() {
    NodeGraphTypeRegistry::registerType< int32_t, "int32_t" >();
}

bool PinMetaObject< int32_t >::drawPinInPropertyGrid( StringView name, int32_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< int64_t >::registerType() {
    NodeGraphTypeRegistry::registerType< int64_t, "int64_t" >();
}

bool PinMetaObject< int64_t >::drawPinInPropertyGrid( StringView name, int64_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint8_t >::registerType() {
    NodeGraphTypeRegistry::registerType< uint8_t, "uint8_t" >();
}

bool PinMetaObject< uint8_t >::drawPinInPropertyGrid( StringView name, uint8_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint16_t >::registerType() {
    NodeGraphTypeRegistry::registerType< uint16_t, "uint8_t" >();
}

bool PinMetaObject< uint16_t >::drawPinInPropertyGrid( StringView name, uint16_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint32_t >::registerType() {
    NodeGraphTypeRegistry::registerType< uint32_t, "uint32_t" >();
}

bool PinMetaObject< uint32_t >::drawPinInPropertyGrid( StringView name, uint32_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< uint64_t >::registerType() {
    NodeGraphTypeRegistry::registerType< uint64_t, "uint64_t" >();
}

bool PinMetaObject< uint64_t >::drawPinInPropertyGrid( StringView name, uint64_t& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< float32 >::registerType() {
    NodeGraphTypeRegistry::registerType< float32, "float32" >();
}

bool PinMetaObject< float32 >::drawPinInPropertyGrid( StringView name, float32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< float64 >::registerType() {
    NodeGraphTypeRegistry::registerType< float64, "float64" >();
}

bool PinMetaObject< float64 >::drawPinInPropertyGrid( StringView name, float64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2s32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector2s32, "vector2s32" >();
}

bool PinMetaObject< Vector2s32 >::drawPinInPropertyGrid( StringView name, Vector2s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2s64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector2s64, "vector2s64" >();
}

bool PinMetaObject< Vector2s64 >::drawPinInPropertyGrid( StringView name, Vector2s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2f32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector2f32, "vector2f32" >();
}

bool PinMetaObject< Vector2f32 >::drawPinInPropertyGrid( StringView name, Vector2f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector2f64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector2f64, "vector2f64" >();
}

bool PinMetaObject< Vector2f64 >::drawPinInPropertyGrid( StringView name, Vector2f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3s32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector3s32, "vector3s32" >();
}

bool PinMetaObject< Vector3s32 >::drawPinInPropertyGrid( StringView name, Vector3s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3s64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector3s64, "vector3s64" >();
}

bool PinMetaObject< Vector3s64 >::drawPinInPropertyGrid( StringView name, Vector3s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3f32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector3f32, "vector3f32" >();
}

bool PinMetaObject< Vector3f32 >::drawPinInPropertyGrid( StringView name, Vector3f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector3f64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector3f64, "vector3f64" >();
}

bool PinMetaObject< Vector3f64 >::drawPinInPropertyGrid( StringView name, Vector3f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4s32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector4s32, "vector4s32" >();
}

bool PinMetaObject< Vector4s32 >::drawPinInPropertyGrid( StringView name, Vector4s32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4s64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector4s64, "vector4s64" >();
}

bool PinMetaObject< Vector4s64 >::drawPinInPropertyGrid( StringView name, Vector4s64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4f32 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector4f32, "vector4f32" >();
}

bool PinMetaObject< Vector4f32 >::drawPinInPropertyGrid( StringView name, Vector4f32& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< Vector4f64 >::registerType() {
    NodeGraphTypeRegistry::registerType< Vector4f64, "vector4f64" >();
}

bool PinMetaObject< Vector4f64 >::drawPinInPropertyGrid( StringView name, Vector4f64& value ) {
    return ui::property_grid::drawProperty( name, value );
}

void PinMetaObject< String >::registerType() {
    NodeGraphTypeRegistry::registerType< String, "string" >();
}

bool PinMetaObject< String >::drawPinInPropertyGrid( StringView name, String& value ) {
    return ui::property_grid::drawProperty( name, value );
}

bool PinMetaObject< rhi::TextureHandle >::drawPinInPropertyGrid( StringView name, rhi::TextureHandle& /*value*/ ) {
    String test = "I'm a texture handle";
    return ui::property_grid::drawProperty( name, test );
}

bool PinMetaObject< rhi::BufferHandle >::drawPinInPropertyGrid( StringView name, rhi::BufferHandle& /*value*/ ) {
    String test = "I'm a buffer handle";
    return ui::property_grid::drawProperty( name, test );
}
} // namespace onyx::node_graph
