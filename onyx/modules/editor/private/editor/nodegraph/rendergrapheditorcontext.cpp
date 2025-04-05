#include <editor/nodegraph/rendergrapheditorcontext.h>

#include <onyx/graphics/rendergraph/rendergraph.h>

namespace Onyx::Editor
{
    
    bool RenderGraphEditorContext::Compile()
    {
        Graph->GetNodeGraph().Compile();
        return true;
    }
}
