#pragma once

#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/pins/pinmeta.h>

namespace onyx::node_graph {
template < typename T >
struct PinMetaObject {
#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, T& value ) {
        return value.DrawPinInPropertyGrid( name, value );
    }

    static constexpr uint32_t GetPinTypeColor() {
        // custom type
        return 0xFFD79633;
    }
#endif
};

template <>
struct PinMetaObject< ExecutePin > {
    static void Register();
#if ONYX_IS_EDITOR
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< bool > {
    static void Register();
#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, bool& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< int8_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, int8_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF4AE293; }
#endif
};

template <>
struct PinMetaObject< int16_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, int16_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< int32_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, int32_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF9CC944; }
#endif
};

template <>
struct PinMetaObject< int64_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, int64_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF228B22; }
#endif
};

template <>
struct PinMetaObject< uint8_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, uint8_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< uint16_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, uint16_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< uint32_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, uint32_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< uint64_t > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, uint64_t& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF3030DC; }
#endif
};

template <>
struct PinMetaObject< float32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, float32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF4AE293; }
#endif
};

template <>
struct PinMetaObject< float64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, float64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFAACD66; }
#endif
};

template <>
struct PinMetaObject< Vector2s32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector2s32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF808000; }
#endif
};

template <>
struct PinMetaObject< Vector2s64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector2s64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF006400; }
#endif
};

template <>
struct PinMetaObject< Vector2f32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector2f32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF90EE90; }
#endif
};

template <>
struct PinMetaObject< Vector2f64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector2f64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFAAB220; }
#endif
};

template <>
struct PinMetaObject< Vector3s32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector3s32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFB48246; }
#endif
};

template <>
struct PinMetaObject< Vector3s64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector3s64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFF8B6436; }
#endif
};

template <>
struct PinMetaObject< Vector3f32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector3f32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFE6D8AD; }
#endif
};

template <>
struct PinMetaObject< Vector3f64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector3f64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFA09E5F; }
#endif
};

template <>
struct PinMetaObject< Vector4s32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector4s32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFD355BA; }
#endif
};

template <>
struct PinMetaObject< Vector4s64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector4s64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFD30094; }
#endif
};

template <>
struct PinMetaObject< Vector4f32 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector4f32& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFDDA0DD; }
#endif
};

template <>
struct PinMetaObject< Vector4f64 > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, Vector4f64& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFCC3299; }
#endif
};

template <>
struct PinMetaObject< String > {
    static void Register();

#if ONYX_IS_EDITOR
    static bool DrawPinInPropertyGrid( StringView name, String& value );
    static constexpr uint32_t GetPinTypeColor() { return 0xFFAA66FF; }
#endif
};
} // namespace onyx::node_graph