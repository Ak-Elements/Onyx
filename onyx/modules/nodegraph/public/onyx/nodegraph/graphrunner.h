#pragma once

#include <onyx/nodegraph/executioncontext.h>

namespace onyx::node_graph {
class NodeGraph;

class GraphRunner {
  public:
    GraphRunner( const NodeGraph& graph )
        : m_graph( &graph ) {}

    void prepare();
    void update( [[maybe_unused]] uint64_t deltaTime );
    void shutdown();

    PrepareContext& getPrepareContext() { return m_prepareContext; }
    ExecutionContext& getContext() { return m_executionContext; }

  private:
    const NodeGraph* m_graph; // should be Ref<GraphAsset>
    PrepareContext m_prepareContext;
    ExecutionContext m_executionContext{ m_prepareContext };
};
} // namespace onyx::node_graph
