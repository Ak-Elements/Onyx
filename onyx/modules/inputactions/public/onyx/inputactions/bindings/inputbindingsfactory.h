#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/bindings/inputbinding.h>

namespace onyx::input_actions
{
    class InputBindingsFactory : public InputTypeFactory<InputBinding>
    {
    };
}
