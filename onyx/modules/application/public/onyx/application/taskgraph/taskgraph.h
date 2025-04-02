#pragma once
#include <onyx/application/application.h>
#include <onyx/application/taskgraph/taskgraphtask.h>
#include <onyx/container/directedacyclicgraph.h>

#include <onyx/entity/typeinfo.h>

namespace Onyx::Application
{
    using DirectedAcyclicTaskGraph = DirectedAcyclicGraph<TaskGraphNode, onyxS16>;

    class TaskGraph
    {
    public:
        TaskGraph();

        void Update(onyxU64 deltaTime, Graphics::FrameContext& context);

        template <typename T, typename... Args>
        onyxS16 AddTask(Args&&... val)
        {
            static_assert(std::is_base_of_v<TaskGraphTask, T>, "T must be derived from TaskGraphTask");
            //constexpr onyxU32 taskGraphHash = Entity::GetHash<T>();
            //ONYX_ASSERT(m_TaskHashToNodeId.contains(taskGraphHash) == false, "TaskGraphTask is already registered.");

            TaskGraphNode newNode(MakeUnique<T>(std::forward<Args>(val)...));
            onyxS16 nodeId = m_Tasks.AddNode(std::move(newNode));
            return nodeId;
        }

        void AddDependency(onyxS16 fromNode, onyxS16 toNode)
        {
            m_Tasks.AddEdge(fromNode, toNode);
        }

        void Init()
        {
            m_Tasks.TransitiveReduction();

            // call on all tasks
            //init
        }

    private:

    private:
        DirectedAcyclicTaskGraph m_Tasks;
    };
}
