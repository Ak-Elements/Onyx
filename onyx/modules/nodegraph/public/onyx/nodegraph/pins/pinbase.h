#pragma once

#include <onyx/filesystem/onyxfile.h>

namespace Onyx::NodeGraph
{
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

        virtual StringId32 GetLocalId() const = 0;
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
}
