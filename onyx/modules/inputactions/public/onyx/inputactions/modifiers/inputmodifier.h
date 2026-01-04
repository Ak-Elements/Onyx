#pragma once

namespace Onyx::InputActions
{
    // modifiers that change the input value
    class InputModifier
    {
    public:
        virtual ~InputModifier() = default;
        virtual StringId32 GetTypeId() const = 0;
        virtual void Apply(Vector3f32& value) const = 0;
    };
}