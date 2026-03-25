#pragma once

namespace onyx::node_graph {
struct ExecutePin {};

enum class PinTypeId : uint32_t {
    Invalid,
    Bool = TypeHash< bool >(),
    Float = TypeHash< float32 >(),
    Double = TypeHash< float64 >(),
    Int32 = TypeHash< int32_t >(),
    Int64 = TypeHash< int64_t >(),
    String = TypeHash< String >(),
    Vector2s32 = TypeHash< Vector2s32 >(),
    Vector2s64 = TypeHash< Vector2s64 >(),
    Vector2f = TypeHash< Vector2f32 >(),
    Vector2d = TypeHash< Vector2f64 >(),
    Vector3s32 = TypeHash< Vector3s32 >(),
    Vector3s64 = TypeHash< Vector3s64 >(),
    Vector3f32 = TypeHash< Vector3f32 >(),
    Vector3f64 = TypeHash< Vector3f64 >(),
    Vector4s32 = TypeHash< Vector4s32 >(),
    Vector4s64 = TypeHash< Vector4s64 >(),
    Vector4f = TypeHash< Vector4f32 >(),
    Vector4d = TypeHash< Vector4f64 >(),
    Execute = TypeHash< ExecutePin >()
};

template < typename T >
concept PinType = requires() {
    { T::LocalId };

    typename T::DataType;
};

class PinBase {
  public:
    PinBase( Guid64 globalPinId )
        : m_GlobalId( globalPinId ) {}

    virtual ~PinBase() = default;

    virtual StringId32 GetLocalId() const = 0;
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    virtual StringView GetLocalIdString() const = 0;
#endif
    virtual PinTypeId GetType() = 0;
    virtual PinTypeId GetType() const = 0;

    Guid64 GetGlobalId() { return m_GlobalId; }
    Guid64 GetGlobalId() const { return m_GlobalId; }

    void SetGlobalId( Guid64 globalId ) { m_GlobalId = globalId; }

    void ConnectPin( Guid64 globalId ) { m_LinkedPinId = globalId; }
    void ClearLink() { m_LinkedPinId = InvalidGuiD64; }
    bool IsConnected() const { return m_LinkedPinId != InvalidGuiD64; }
    Guid64 GetLinkedPinGlobalId() const { return m_LinkedPinId; }

    virtual std::any CreateDefault() const = 0;

#if ONYX_IS_EDITOR
    virtual void DrawPropertyPanel( StringView name, std::any& anyValue ) const = 0;
    constexpr virtual uint32_t GetTypeColor() const = 0;
#endif

  private:
    Guid64 m_GlobalId;
    Guid64 m_LinkedPinId;
};
} // namespace onyx::node_graph
