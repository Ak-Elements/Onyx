#pragma once

#include <onyx/inputactions/inputtypefactory.h>
#include <onyx/inputactions/triggers/inputtrigger.h>

namespace Onyx::InputActions
{
    class InputTriggersFactory : public InputTypeFactory<InputTrigger>
    {
    };
}
