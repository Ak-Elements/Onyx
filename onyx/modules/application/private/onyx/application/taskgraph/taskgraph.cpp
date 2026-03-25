#include <onyx/application/taskgraph/taskgraph.h>

namespace onyx::application {
TaskGraph::TaskGraph() = default;

void TaskGraph::update( uint64_t deltaTime, rhi::FrameContext& context ) {
    if ( m_tasks.getCount() == 0 )
        return;

    DynamicArray< DirectedAcyclicTaskGraph::NodeId > sortedTasks;
    m_tasks.retrieveTopologicalOrder( sortedTasks );

    for ( DirectedAcyclicTaskGraph::NodeId nodeId : sortedTasks ) {
        TaskGraphNode& task = m_tasks.getNode( nodeId );
        task.update( deltaTime, context );
    }
}
} // namespace onyx::application
