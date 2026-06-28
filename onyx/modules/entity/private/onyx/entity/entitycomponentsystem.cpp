#include <onyx/entity/entitycomponentsystem.h>

namespace onyx::ecs {
void EntityComponentSystemsGraph::update( const ECSExecutionContext& context ) const {
    for( const auto& system : m_systems ) {
        system( context );
    }
}
} // namespace onyx::ecs
