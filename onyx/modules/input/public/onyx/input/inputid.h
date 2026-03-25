#pragma once

namespace onyx::input {
enum class InputType : uint8_t;
enum class MouseButton : uint16_t;
enum class MouseAxis : uint16_t;
enum class Key : uint16_t;
enum class GameControllerAxis : uint16_t;
enum class GameControllerButton : uint16_t;

template < typename T >
concept InputEnum = std::is_same_v< T, MouseButton > || std::is_same_v< T, MouseAxis > || std::is_same_v< T, Key > ||
                    std::is_same_v< T, GameControllerAxis > || std::is_same_v< T, GameControllerButton >;

static constexpr uint16_t InvalidInputID = 0;

struct InputID {
    static constexpr uint16_t Invalid = 0;
    uint16_t ID = Invalid;

    InputID() = default;

    template < InputEnum T >
    InputID( T inputValue )
        : ID( enums::toIntegral( inputValue ) ) {}

    void operator=( uint16_t other ) { ID = other; }
    void operator=( InputID other ) { ID = other.ID; }

    bool operator==( uint16_t other ) const { return ID == other; }
    bool operator!=( uint16_t other ) const { return ID != other; }
};

bool IsAxis1D( InputID id );
bool IsAxis2D( InputID id );
bool IsMouseButton( InputID id );
bool IsMouseAxis1D( InputID id );
bool IsMouseAxis2D( InputID id );
bool IsKeyboardKey( InputID id );
bool IsGameControllerButton( InputID id );
bool IsGameControllerAxis1D( InputID id );
bool IsGameControllerAxis2D( InputID id );

StringView ToString( InputID id );
StringView GetInputTypeString( InputID id );
InputType GetInputType( InputID id );
} // namespace onyx::input

namespace onyx {
template <>
struct Serialization< input::InputID > {
    static bool serialize( Serializer& serializer, const input::InputID& id );
    static bool deserialize( const Deserializer& deserializer, input::InputID& outId );
};
} // namespace onyx