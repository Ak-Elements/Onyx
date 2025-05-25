#pragma once

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

        static bool Serialize(FileSystem::JsonValue& json, const T& value)
        {
            return value.Serialize(json);
        }

        static bool Deserialize(const FileSystem::JsonValue& json, T& value)
        {
            return value.Deserialize(json);
        }
    };

    template <>
    struct PinMetaObject<bool>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, bool& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif

        static bool Serialize(FileSystem::JsonValue& json, bool value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, bool& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxS8>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS8& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF4AE293; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxS8 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxS8& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxS16>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS16& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxS16 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxS16& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxS32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF9CC944; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxS32 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxS32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxS64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxS64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF228B22; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxS64 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxS64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxU8>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU8& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxU8 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxU8& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxU16>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU16& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxU16 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxU16& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxU32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxU32 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxU32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxU64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxU64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF3030DC; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxU64 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxU64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxF32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxF32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF4AE293; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxF32 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxF32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<onyxF64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, onyxF64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAACD66; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, onyxF64 value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, onyxF64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector2s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF808000; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector2s32& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector2s32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector2s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF006400; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector2s64& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector2s64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector2f>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2f& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF90EE90; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector2f& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector2f& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector2d>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector2d& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAAB220; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector2d& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector2d& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector3s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFB48246; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector3s32& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector3s32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector3s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFF8B6436; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector3s64& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector3s64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector3f>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3f& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFE6D8AD; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector3f& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector3f& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector3d>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector3d& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFA09E5F; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector3d& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector3d& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector4s32>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4s32& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFD355BA; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector4s32& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector4s32& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector4s64>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4s64& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFD30094; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector4s64& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector4s64& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector4f>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4f& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFDDA0DD; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector4f& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector4f& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<Vector4d>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, Vector4d& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFCC3299; }
#endif
        static bool Serialize(FileSystem::JsonValue& json, const Vector4d& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, Vector4d& value)
        {
            json.Get("data", value);
            return true;
        }
    };

    template <>
    struct PinMetaObject<String>
    {
#if ONYX_IS_EDITOR
        static bool DrawPinInPropertyGrid(StringView name, String& value);
        static constexpr onyxU32 GetPinTypeColor() { return 0xFFAA66FF; }
#endif

        static bool Serialize(FileSystem::JsonValue& json, const String& value)
        {
            json.Set("data", value);
            return true;
        }

        static bool Deserialize(const FileSystem::JsonValue& json, String& value)
        {
            json.Get("data", value);
            return true;
        }
    };
}