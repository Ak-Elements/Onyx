#pragma once

#include <any>

#include <onyx/filesystem/onyxfile.h>

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

    struct ExecutePin {};

    enum class PinTypeId : onyxU32
    {
        Invalid,
        Bool = TypeHash<bool>(),
        Float = TypeHash<onyxF32>(),
        Double = TypeHash<onyxF64>(),
        Int32 = TypeHash<onyxS32>(),
        Int64 = TypeHash<onyxS64>(),
        String = TypeHash<String>(),
        Vector2s32 = TypeHash<Vector2s32>(),
        Vector2s64 = TypeHash<Vector2s64>(),
        Vector2f = TypeHash<Vector2f>(),
        Vector2d = TypeHash<Vector2d>(),
        Vector3s32 = TypeHash<Vector3s32>(),
        Vector3s64 = TypeHash<Vector3s64>(),
        Vector3f = TypeHash<Vector3f>(),
        Vector3d = TypeHash<Vector3d>(),
        Vector4s32 = TypeHash<Vector4s32>(),
        Vector4s64 = TypeHash<Vector4s64>(),
        Vector4f = TypeHash<Vector4f>(),
        Vector4d = TypeHash<Vector4d>(),
        Execute = TypeHash<ExecutePin>()
    };

    template <typename T>
    concept PinType = requires()
    {
        { T::LocalId };
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        { T::LocalIdString };
#endif

        typename T::DataType;
    };

    class PinBase
    {
    public:
        PinBase(Guid64 globalPinId)
            : m_GlobalId(globalPinId)
        {
        }

        virtual ~PinBase() = default;

        virtual onyxU32 GetLocalId() const = 0;
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        virtual StringView GetLocalIdString() const = 0;
#endif
        virtual PinTypeId GetType() = 0;
        virtual PinTypeId GetType() const = 0;

        Guid64 GetGlobalId() { return m_GlobalId; }
        Guid64 GetGlobalId() const { return m_GlobalId; }

        void SetGlobalId(Guid64 globalId) { m_GlobalId = globalId; }

        void ConnectPin(Guid64 globalId) { m_LinkedPinId = globalId; }
        void ClearLink() { m_LinkedPinId = INVALID_GUID64; }
        bool IsConnected() const { return m_LinkedPinId != INVALID_GUID64; }
        Guid64 GetLinkedPinGlobalId() const { return m_LinkedPinId; }

        virtual std::any CreateDefault() const = 0;

#if ONYX_IS_EDITOR
        virtual void DrawPropertyPanel(StringView name, std::any& anyValue) const = 0;
        constexpr virtual onyxU32 GetTypeColor() const = 0;
#endif

        virtual bool Serialize(FileSystem::JsonValue& json, const std::any& anyValue) const = 0;
        virtual bool Deserialize(const FileSystem::JsonValue& json, std::any& anyValue) = 0;

    private:
        Guid64 m_GlobalId;
        Guid64 m_LinkedPinId;
    };

    template <typename DataT>
    class DynamicPin : public PinBase
    {
    public:
        using DataType = DataT;

        DynamicPin(const String& id)
            : PinBase(Guid64Generator::GetGuid())
            , LocalId(Hash::FNV1aHash32(id))
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
            , LocalIdString(id)
#endif
        {
        }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        DynamicPin(Guid64 globalPinId, const String& id)
            : PinBase(globalPinId)
            , LocalId(Hash::FNV1aHash32(id))
            , LocalIdString(id)

        {
        }

        DynamicPin(Guid64 globalPinId, onyxU32 localId, const String& id)
            : PinBase(globalPinId)
            , LocalId(localId)
            , LocalIdString(id)
        {
        }
#endif

        onyxU32 LocalId = 0;
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        String LocalIdString;
#endif
        static constexpr PinTypeId DataTypeId = static_cast<PinTypeId>(TypeHash<DataT>());

        std::any CreateDefault() const override { return DataT(); }

#if ONYX_IS_EDITOR

        void DrawPropertyPanel(StringView name, std::any& anyValue) const override;
        constexpr onyxU32 GetTypeColor() const override { return PinMetaObject<DataT>::GetPinTypeColor(); }
#endif

        bool Serialize(FileSystem::JsonValue& json, const std::any& anyValue) const override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                const DataT& value = std::any_cast<const DataT&>(anyValue);
                return PinMetaObject<DataT>::Serialize(json, value);
            }
            else
            {
                return true;
            }
        }

        bool Deserialize(const FileSystem::JsonValue& json, std::any& anyValue) override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                DataT data;
                if (PinMetaObject<DataT>::Deserialize(json, data))
                {
                    anyValue = data;
                    return true;
                }

                return false;
            }
            else
            {
                return true;
            }
        }

        onyxU32 GetLocalId() const override { return LocalId; }
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        StringView GetLocalIdString() const override { return LocalIdString; }
#endif

        PinTypeId GetType() override { return DataTypeId; }
        PinTypeId GetType() const override { return DataTypeId; }
    };

    template <typename DataT, CompileTimeString PinId>
    class Pin : public PinBase
    {
    public:
        using DataType = DataT;
        static constexpr onyxU32 LocalId = Hash::FNV1aHash32(PinId.data(), PinId.size());
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        static constexpr StringView LocalIdString{ PinId.data(), PinId.size() };
#endif
        static constexpr PinTypeId DataTypeId = static_cast<PinTypeId>(TypeHash<DataT>());

        Pin()
            : PinBase(Guid64Generator::GetGuid())
        {
        }

        Pin(Guid64 globalPinId)
            : PinBase(globalPinId)
        {
        }

        std::any CreateDefault() const override { return DataT(); }

#if ONYX_IS_EDITOR
        void DrawPropertyPanel(StringView name, std::any& anyValue) const override;
        constexpr onyxU32 GetTypeColor() const override { return PinMetaObject<DataT>::GetPinTypeColor(); }
#endif

        bool Serialize(FileSystem::JsonValue& json, const std::any& anyValue) const override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                const DataT& value = std::any_cast<const DataT&>(anyValue);
                return PinMetaObject<DataT>::Serialize(json, value);
            }
            else
            {
                return true;
            }
        }

        bool Deserialize(const FileSystem::JsonValue& json, std::any& anyValue) override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                DataT data;
                if (PinMetaObject<DataT>::Deserialize(json, data))
                {
                    anyValue = data;
                    return true;
                }

                return false;
            }
            else
            {
                return true;
            }
        }

        onyxU32 GetLocalId() const override { return LocalId; }
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        StringView GetLocalIdString() const override { return LocalIdString; }
#endif

        PinTypeId GetType() override { return DataTypeId; }
        PinTypeId GetType() const override { return DataTypeId; }
    };

#if ONYX_IS_EDITOR
    template <typename DataT>
    void DynamicPin<DataT>::DrawPropertyPanel(StringView name, std::any& anyValue) const
    {
        if constexpr (std::is_same_v<DataT, ExecutePin> == false)
        {
            DataT& value = std::any_cast<DataT&>(anyValue);
            PinMetaObject<DataT>::DrawPinInPropertyGrid(name, value);
        }
    }

    template <typename DataT, CompileTimeString PinId>
    void Pin<DataT, PinId>::DrawPropertyPanel(StringView name, std::any& anyValue) const
    {
        if constexpr(std::is_same_v<DataT, ExecutePin> == false)
        {
            DataT& value = std::any_cast<DataT&>(anyValue);
            PinMetaObject<DataT>::DrawPinInPropertyGrid(name, value);
        }
    }
#endif

    UniquePtr<PinBase> CreatePin(PinTypeId typeId, Guid64 globalId, onyxU32 localId, const String& localIdString);
}
