#include <onyx/application/taskgraph/taskgraph.h>

namespace onyx::application
{
    TaskGraph::TaskGraph()
    {
    }

    void TaskGraph::Update(onyxU64 deltaTime, rhi::FrameContext& context)
    {
        if (m_Tasks.GetCount() == 0)
            return;

        DynamicArray<DirectedAcyclicTaskGraph::NodeId> sortedTasks;
        m_Tasks.RetrieveTopologicalOrder(sortedTasks);

        for (DirectedAcyclicTaskGraph::NodeId nodeId : sortedTasks)
        {
            TaskGraphNode& task = m_Tasks.GetNode(nodeId);
            task.Update(deltaTime, context);
        }
    }
}
