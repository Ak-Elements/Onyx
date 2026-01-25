#include <onyx/inputactions/bindings/inputbindingsfactory.h>

namespace Onyx::InputActions
{
    HashMap<StringId32, InputBindingsFactory::MetaData> InputBindingsFactory::s_RegisteredBindings;
}
