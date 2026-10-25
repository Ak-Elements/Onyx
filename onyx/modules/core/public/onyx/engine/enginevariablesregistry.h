#pragma once

#include <onyx/engine/enginevariable.h>
#include <onyx/serialize/serialization.h>

namespace onyx {
class EngineVariablesRegistry {
    friend struct Serialization< EngineVariablesRegistry >;

  public:
    void registerVariable( IEngineVariable& variable ) { m_variables.emplace( variable.getId(), &variable ); }
    void unregisterVariable( IEngineVariable& variable ) { m_variables.erase( variable.getId() ); }

    const HashMap< StringId32, IEngineVariable* >& getVariables() const { return m_variables; }

  private:
    HashMap< StringId32, IEngineVariable* > m_variables;
};

template <>
struct Serialization< EngineVariablesRegistry > {
    static bool serialize( Serializer& serializer, const EngineVariablesRegistry& registry );
    static bool deserialize( const Deserializer& deserializer, EngineVariablesRegistry& outRegistry );
};
} // namespace onyx
