#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>

#include <onyx/graphics/rendergraph/rendergraph.h>

namespace onyx::editor {

bool RenderGraphEditorContext::compile() {
    m_graph->GetNodeGraph().compile();
    return true;
}
} // namespace onyx::editor
