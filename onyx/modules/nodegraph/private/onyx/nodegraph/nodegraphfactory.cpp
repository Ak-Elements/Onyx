#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::NodeGraph
{
    NodeRegistry<NodeEditorMetaData> TypedNodeFactory<Node, NodeEditorMetaData>::ms_NodeRegistry;
}