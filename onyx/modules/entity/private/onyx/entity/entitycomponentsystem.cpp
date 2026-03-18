#include <onyx/entity/entitycomponentsystem.h>

namespace onyx::ecs
{
    void EntityComponentSystemsGraph::Update(const ECSExecutionContext& context) const
    {
        for (const auto& system : m_Systems)
        {
            system(context);
        }
    }
}
