#pragma once

#include <onyx/engine/enginevariable.h>

namespace onyx
{
    class EngineVariablesRegistry
    {
    public:
        void Register(IEngineVariable& variable) { m_Variables.emplace(variable.GetId(), &variable); }
        void Unregister(IEngineVariable& variable) { m_Variables.erase(variable.GetId()); }

        const HashMap<StringId32, IEngineVariable*> GetVariables() const { return m_Variables; }
    private:
        HashMap<StringId32, IEngineVariable*> m_Variables;
    };
}