#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/bindings/inputbinding.h>

namespace Onyx::InputActions
{
    class InputBindingsFactory : public InputTypeFactory<InputBinding>
    {
    };
}
