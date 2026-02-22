#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>

namespace Onyx::InputActions
{
    class InputModifiersFactory : public InputTypeFactory<InputModifier>
    {
    };
}
