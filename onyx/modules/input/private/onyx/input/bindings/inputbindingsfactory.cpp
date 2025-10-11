#include <onyx/input/bindings/inputbindingsfactory.h>

namespace Onyx::Input
{
    HashMap<StringId32, InputBindingsFactory::MetaData> InputBindingsFactory::s_RegisteredBindings;
}
