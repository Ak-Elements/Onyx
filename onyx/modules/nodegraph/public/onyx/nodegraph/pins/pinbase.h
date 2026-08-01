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
        : m_globalId( globalPinId ) {}

    virtual ~PinBase() = default;

    [[nodiscard]] virtual StringId32 getLocalId() const = 0;
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
    [[nodiscard]] virtual StringView getLocalIdString() const = 0;
#endif
    virtual PinTypeId getType() = 0;
    [[nodiscard]] virtual PinTypeId getType() const = 0;

    Guid64 getGlobalId() { return m_globalId; }
    [[nodiscard]] Guid64 getGlobalId() const { return m_globalId; }

    void setGlobalId( Guid64 globalId ) { m_globalId = globalId; }

    void connectPin( Guid64 globalId ) { m_linkedPinId = globalId; }
    void clearLink() { m_linkedPinId = InvalidGuiD64; }
    [[nodiscard]] bool isConnected() const { return m_linkedPinId != InvalidGuiD64; }
    [[nodiscard]] Guid64 getLinkedPinGlobalId() const { return m_linkedPinId; }

    [[nodiscard]] virtual std::any createDefault() const = 0;

#if ONYX_IS_EDITOR
    virtual void drawPropertyPanel( StringView name, std::any& anyValue ) const = 0;
    [[nodiscard]] constexpr virtual uint32_t getTypeColor() const = 0;
#endif

  private:
    Guid64 m_globalId;
    Guid64 m_linkedPinId;
};
} // namespace onyx::node_graph
