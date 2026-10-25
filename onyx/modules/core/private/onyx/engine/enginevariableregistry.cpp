#include <onyx/engine/enginevariablesregistry.h>

#include <onyx/serialize/deserializer.h>
#include <onyx/serialize/serializer.h>

namespace onyx {
bool Serialization< EngineVariablesRegistry >::serialize( Serializer& serializer,
                                                          const EngineVariablesRegistry& registry ) {
    // Do not serialize the entire registry
    return true;
}

bool Serialization< EngineVariablesRegistry >::deserialize( const Deserializer& deserializer,
                                                            EngineVariablesRegistry& outRegistry ) {
    deserializer.readForEach( [ & ]( const Deserializer& scopedDeserializer ) {
        StringView variableName;
        scopedDeserializer.getScopeIdentifier( variableName );

        StringId32 engineVariableId( variableName );
        auto it = outRegistry.m_variables.find( engineVariableId );

        if( it != outRegistry.m_variables.end() ) {
            it->second->deserialize( scopedDeserializer );
        }

        return true;
    } );
    return true;
}
} // namespace onyx
