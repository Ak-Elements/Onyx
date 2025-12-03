#pragma once

#include <onyx/editor/nodegraph/grapheditorcontext.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/graph.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>
#include <onyx/ui/controls/treeview.h>

namespace Onyx::Editor
{
    class NodeGraphEditorContext : public GraphEditorContext
    {
    public:
        bool IsNewLinkValid(Guid64 fromPinId, Guid64 toPinId) const override;

        bool ArePinTypesCompatible(NodeGraph::PinTypeId lhsPinType, NodeGraph::PinTypeId rhsPinType) const override;

        void DrawNodeInPropertyPanel(Guid64 nodeId) override;
        
        void FilterNodeListContextMenu(InplaceFunction<bool(StringView, const NodeGraph::NodeEditorMetaData& nodeMeta)> filterFunctor) override;
        void ClearNodeListFilter() override;
        const Ui::TreeItem& GetNodeListContextMenuRoot() override;

    protected:
        void UpdateEditorNodeData(Node& editorNode, const NodeGraph::Node& graphNode);
        virtual NodeGraph::NodeGraph& GetNodeGraph() = 0;
        virtual const NodeGraph::NodeGraph& GetNodeGraph() const = 0;

        template <typename T = NodeGraph::Node>
        T& GetNodeGraphNode(NodeGraph::NodeGraph::LocalNodeId localNodeId)
        {
            return GetNodeGraph().GetNode(localNodeId);
        }

        template <typename T = NodeGraph::Node>
        const T& GetNodeGraphNode(NodeGraph::NodeGraph::LocalNodeId localNodeId) const
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
        Ui::TreeItem m_ContextMenuRoot;
    };
}
