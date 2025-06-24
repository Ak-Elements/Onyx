#pragma once

#include <onyx/nodegraph/pins/pinmeta.h>

namespace Onyx::NodeGraph
{
    template <typename T>
    struct PinMetaObject
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, T& value)
        {
            return value.DrawPinInPropertyGrid(name, value);
        }

        static constexpr onyxU32 GetPinTypeColor()
        {
            //custom type
            return 0xFFD79633;
        }
#endif
    };

    template <>
    struct PinMetaObject<bool>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, bool& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
    };

    template <>
    struct PinMetaObject<onyxS8>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS8& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF4AE293; }
#endif
    };

    template <>
    struct PinMetaObject<onyxS16>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS16& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif

    };

    template <>
    struct PinMetaObject<onyxS32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF9CC944; }
#endif
       
    };

    template <>
    struct PinMetaObject<onyxS64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF228B22; }
#endif
        
    };

    template <>
    struct PinMetaObject<onyxU8>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU8& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        
    };

    template <>
    struct PinMetaObject<onyxU16>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU16& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        
    };

    template <>
    struct PinMetaObject<onyxU32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
       
    };

    template <>
    struct PinMetaObject<onyxU64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        
    };

    template <>
    struct PinMetaObject<onyxF32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxF32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF4AE293; }
#endif
    };

    template <>
    struct PinMetaObject<onyxF64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxF64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAACD66; }
#endif
    };

    template <>
    struct PinMetaObject<Vector2s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF808000; }
#endif
    };

    template <>
    struct PinMetaObject<Vector2s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF006400; }
#endif
    };

    template <>
    struct PinMetaObject<Vector2f32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2f32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF90EE90; }
#endif
    };

    template <>
    struct PinMetaObject<Vector2f64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2f64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAAB220; }
#endif
    };

    template <>
    struct PinMetaObject<Vector3s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; }
#endif
    };

    template <>
    struct PinMetaObject<Vector3s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF8B6436; }
#endif
    };

    template <>
    struct PinMetaObject<Vector3f32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3f32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFE6D8AD; }
#endif
    };

    template <>
    struct PinMetaObject<Vector3f64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3f64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFA09E5F; }
#endif
    };

    template <>
    struct PinMetaObject<Vector4s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFD355BA; }
#endif
    };

    template <>
    struct PinMetaObject<Vector4s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFD30094; }
#endif
    };

    template <>
    struct PinMetaObject<Vector4f32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4f32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFDDA0DD; }
#endif
    };

    template <>
    struct PinMetaObject<Vector4f64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4f64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFCC3299; }
#endif
    };

    template <>
    struct PinMetaObject<String>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, String& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAA66FF; }
#endif
    };
}