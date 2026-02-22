#pragma once

namespace Onyx::Input
{
    enum class InputType : onyxU8;
    enum class MouseButton : onyxU16;
    enum class MouseAxis : onyxU16;
    enum class Key : onyxU16;
    enum class GameControllerAxis : onyxU16;
    enum class GameControllerButton : onyxU16;
    
    template <typename T>
    concept InputEnum = std::is_same_v<T, MouseButton> ||
        std::is_same_v<T, MouseAxis> ||
        std::is_same_v<T, Key> ||
        std::is_same_v<T, GameControllerAxis> ||
        std::is_same_v<T, GameControllerButton>;

    static constexpr onyxU16 InvalidInputID = 0;

    struct InputID
    {
        static constexpr onyxU16 Invalid = 0;
        onyxU16 ID = Invalid;

        InputID() = default;
        
        template <InputEnum T>
        InputID(T inputValue)
            : ID(Enums::ToIntegral(inputValue))
        {
        }
        
        void operator=(onyxU16 other) { ID = other; }
        void operator=(InputID other) { ID = other.ID; }
        
        bool operator==(onyxU16 other) const { return ID == other; }
        bool operator!=(onyxU16 other) const { return ID != other; }
    };

    bool IsAxis1D(InputID id);
    bool IsAxis2D(InputID id);
    bool IsMouseButton(InputID id);
    bool IsMouseAxis1D(InputID id);
    bool IsMouseAxis2D(InputID id);
    bool IsKeyboardKey(InputID id);
    bool IsGameControllerButton(InputID id);
    bool IsGameControllerAxis1D(InputID id);
    bool IsGameControllerAxis2D(InputID id);

    StringView ToString(InputID id);
    StringView GetInputTypeString(InputID id);
    InputType GetInputType(InputID id);
}

namespace Onyx
{
    template <>
    struct Serialization<Input::InputID>
    {
        static bool Serialize(Serializer& serializer, const Input::InputID& id);
        static bool Deserialize(const Deserializer& deserializer, Input::InputID& outId);
    };
}