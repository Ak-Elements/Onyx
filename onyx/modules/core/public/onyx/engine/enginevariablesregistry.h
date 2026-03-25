#pragma once

#include <onyx/engine/enginevariable.h>

namespace onyx {
class EngineVariablesRegistry {
  public:
    void registerVariable( IEngineVariable& variable ) { m_variables.emplace( variable.getId(), &variable ); }
    void unregisterVariable( IEngineVariable& variable ) { m_variables.erase( variable.getId() ); }

    const HashMap< StringId32, IEngineVariable* >& getVariables() const { return m_variables; }

  private:
    HashMap< StringId32, IEngineVariable* > m_variables;
};
} // namespace onyx