#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>

namespace onyx::input_actions
{
    class InputModifiersFactory : public InputTypeFactory<InputModifier>
    {
    };
}
