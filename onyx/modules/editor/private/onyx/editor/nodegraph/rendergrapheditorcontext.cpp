#include <onyx/editor/nodegraph/rendergrapheditorcontext.h>

#include <onyx/graphics/rendergraph/rendergraph.h>

namespace onyx::editor
{
    
    bool RenderGraphEditorContext::Compile()
    {
        Graph->GetNodeGraph().Compile();
        return true;
    }
}
