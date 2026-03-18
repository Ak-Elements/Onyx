#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/triggers/inputtrigger.h>

namespace onyx::input_actions
{
    class InputTriggersFactory : public InputTypeFactory<InputTrigger>
    {
    };
}
