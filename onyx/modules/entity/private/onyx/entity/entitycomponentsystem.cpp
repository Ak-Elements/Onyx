#include <onyx/entity/entitycomponentsystem.h>

namespace Onyx::Entity
{
    void EntityComponentSystemsGraph::Update(const ECSExecutionContext& context) const
    {
        for (const auto& system : m_Systems)
        {
            system(context);
        }
    }
}
