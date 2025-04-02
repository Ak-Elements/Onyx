#pragma once

#include <onyx/nodegraph/executioncontext.h>

namespace Onyx::NodeGraph
{
    class Node;
    class NodeGraph;

    class GraphRunner
    {
    public:
        GraphRunner(const NodeGraph& graph)
            : m_Graph(&graph)
        {
        }

        void Prepare();
        void Update(onyxU64 deltaTime);
        void Shutdown();

        PrepareContext& GetPrepareContext() { return m_PrepareContext; }
        ExecutionContext& GetContext() { return m_ExecutionContext; }

    private:
        const NodeGraph* m_Graph; // should be Ref<GraphAsset>
        PrepareContext m_PrepareContext;
        ExecutionContext m_ExecutionContext { m_PrepareContext };
    };
}
