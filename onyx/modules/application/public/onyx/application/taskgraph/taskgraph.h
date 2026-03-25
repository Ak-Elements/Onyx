#pragma once
#include <onyx/application/application.h>
#include <onyx/application/taskgraph/taskgraphtask.h>
#include <onyx/container/directedacyclicgraph.h>

namespace onyx::application {
using DirectedAcyclicTaskGraph = DirectedAcyclicGraph< TaskGraphNode, int16_t >;

class TaskGraph {
  public:
    TaskGraph();

    void update( uint64_t deltaTime, rhi::FrameContext& context );

    template < typename T, typename... Args > int16_t addTask( Args&&... val ) {
        static_assert( std::is_base_of_v< TaskGraphTask, T >, "T must be derived from TaskGraphTask" );
        // constexpr uint32_t taskGraphHash = ecs::GetHash<T>();
        // ONYX_ASSERT(m_TaskHashToNodeId.contains(taskGraphHash) == false, "TaskGraphTask is already registered.");

        TaskGraphNode newNode( makeUnique< T >( std::forward< Args >( val )... ) );
        int16_t nodeId = m_tasks.addNode( std::move( newNode ) );
        return nodeId;
    }

    void addDependency( int16_t fromNode, int16_t toNode ) { m_tasks.addEdge( fromNode, toNode ); }

    void init() {
        m_tasks.transitiveReduction();

        // call on all tasks
        // init
    }

  private:
  private:
    DirectedAcyclicTaskGraph m_tasks;
};
} // namespace onyx::application
