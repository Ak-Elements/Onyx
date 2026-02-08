#include <onyx/engine/enginevariable.h>

#include <onyx/engine/enginesystem.h>

namespace Onyx
{
    IEngineVariable::IEngineVariable(StringId32 id)
        : m_Id(id)
    {
        IEngine::Register(*this);
    }

    IEngineVariable::~IEngineVariable()
    {
        IEngine::Unregister(*this);
    }

}