#include <onyx/graphics/shadergraph/shadergraphnodefactory.h>

namespace Onyx::NodeGraph
{
    NodeRegistry<Graphics::ShaderNodeMetaData> TypedNodeFactory<Graphics::ShaderGraphNode, Graphics::ShaderNodeMetaData>::ms_NodeRegistry;
}
