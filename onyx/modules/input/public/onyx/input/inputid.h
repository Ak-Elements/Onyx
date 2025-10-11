#pragma once

namespace Onyx::Input
{
    enum class InputType : onyxU8;

    static constexpr onyxU16 InvalidInputID = 0;

    struct InputID
    {
        static constexpr onyxU16 Invalid = 0;
        onyxU16 ID = Invalid;

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