#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>
#include <onyx/nodegraph/nodefactory.h>

namespace Onyx::NodeGraph
{
    NodeRegistry<Graphics::RenderGraphNodeEditorMetaData> TypedNodeFactory<Graphics::IRenderGraphNode, Graphics::RenderGraphNodeEditorMetaData>::ms_NodeRegistry;
}