#pragma once

#include <onyx/assets/assetsystem.h>
#include <onyx/editor/nodegraph/grapheditorcontext.h>
#include <onyx/nodegraph/graph.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/ui/controls/treeview.h>

namespace onyx::editor {
class NodeGraphEditorContext : public GraphEditorContext {
  public:
    bool isNewLinkValid( Guid64 fromPinId, Guid64 toPinId ) const override;

    bool arePinTypesCompatible( node_graph::PinTypeId lhsPinType, node_graph::PinTypeId rhsPinType ) const override;

    void drawNodeInPropertyPanel( Guid64 nodeId ) override;

    void filterNodeListContextMenu(
        InplaceFunction< bool( StringView, const node_graph::NodeEditorMetaData& nodeMeta ) > filterFunctor ) override;
    void clearNodeListFilter() override;
    const ui::TreeItem& getNodeListContextMenuRoot() override;

  protected:
    void updateEditorNodeData( Node& editorNode, const node_graph::Node& graphNode );
    virtual node_graph::NodeGraph& getNodeGraph() = 0;
    virtual const node_graph::NodeGraph& getNodeGraph() const = 0;

    template < typename T = node_graph::Node >
    T& getNodeGraphNode( node_graph::NodeGraph::LocalNodeId localNodeId ) {
        return getNodeGraph().getNode( localNodeId );
    }

    template < typename T = node_graph::Node >
    const T& getNodeGraphNode( node_graph::NodeGraph::LocalNodeId localNodeId ) const {
        return getNodeGraph().getNode( localNodeId );
    }

  private:
    void onDrawNode( const Node& node ) override;
    void onDrawNodeBackground( const Node& node ) override;

    bool onNodeCreate( Node& newEditorNode, StringId32 typeId ) override;
    void onNodeDelete( Node& nodeToDelete ) override;
    void onLinkCreate( const Link& newLink ) override;
    void onLinkDelete( const Link& linkToDelete ) override;

  protected:
    ui::TreeItem m_contextMenuRoot;
};
} // namespace onyx::editor
