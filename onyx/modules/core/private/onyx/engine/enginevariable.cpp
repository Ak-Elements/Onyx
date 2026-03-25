#include <onyx/engine/enginevariable.h>

#include <onyx/engine/enginesystem.h>

namespace onyx {
IEngineVariable::IEngineVariable( StringId32 id )
    : m_id( id ) {
    IEngine::registerVariable( *this );
}

IEngineVariable::~IEngineVariable() {
    IEngine::unregisterVariable( *this );
}

} // namespace onyx