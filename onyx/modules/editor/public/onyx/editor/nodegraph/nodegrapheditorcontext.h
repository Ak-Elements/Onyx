#pragma once

#include <onyx/editor/nodegraph/grapheditorcontext.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/graph.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/ui/controls/treeview.h>

namespace onyx::editor
{
    class NodeGraphEditorContext : public GraphEditorContext
    {
    public:
        bool IsNewLinkValid(Guid64 fromPinId, Guid64 toPinId) const override;

        bool ArePinTypesCompatible(node_graph::PinTypeId lhsPinType, node_graph::PinTypeId rhsPinType) const override;

        void DrawNodeInPropertyPanel(Guid64 nodeId) override;
        
        void FilterNodeListContextMenu(InplaceFunction<bool(StringView, const node_graph::NodeEditorMetaData& nodeMeta)> filterFunctor) override;
        void ClearNodeListFilter() override;
        const ui::TreeItem& GetNodeListContextMenuRoot() override;

    protected:
        void UpdateEditorNodeData(Node& editorNode, const node_graph::Node& graphNode);
        virtual node_graph::NodeGraph& GetNodeGraph() = 0;
        virtual const node_graph::NodeGraph& GetNodeGraph() const = 0;

        template <typename T = node_graph::Node>
        T& GetNodeGraphNode(node_graph::NodeGraph::LocalNodeId localNodeId)
        {
            return GetNodeGraph().GetNode(localNodeId);
        }

        template <typename T = node_graph::Node>
        const T& GetNodeGraphNode(node_graph::NodeGraph::LocalNodeId localNodeId) const
        {
            return GetNodeGraph().GetNode(localNodeId);
        }

    private:
        void OnDrawNode(const Node& node) override;
        void OnDrawNodeBackground(const Node& node) override;

        bool OnNodeCreate(Node& newEditorNode, StringId32 typeId) override;
        void OnNodeDelete(Node& nodeToDelete) override;
        void OnLinkCreate(const Link& newLink) override;
        void OnLinkDelete(const Link& linkToDelete) override;

    protected:
        ui::TreeItem m_ContextMenuRoot;
    };
}
