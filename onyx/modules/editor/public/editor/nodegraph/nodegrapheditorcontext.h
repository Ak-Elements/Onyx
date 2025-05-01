#pragma once

#include <editor/nodegraph/grapheditorcontext.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/graph.h>

#include <onyx/graphics/shadergraph/materialshadergraph.h>

namespace Onyx::Editor
{
    class NodeGraphEditorContext : public GraphEditorContext
    {
    public:
        bool IsNewLinkValid(Guid64 fromPinId, Guid64 toPinId) const override;

        bool ArePinTypesCompatible(NodeGraph::PinTypeId lhsPinType, NodeGraph::PinTypeId rhsPinType) const override;

        void DrawNodeInPropertyPanel(Guid64 nodeId) override;
        
        void FilterNodeListContextMenu(InplaceFunction<bool(const NodeGraph::NodeEditorMetaData& nodeMeta)> filterFunctor) override;
        void ClearNodeListFilter() override;
        const NodeListContextMenuItem& GetNodeListContextMenuRoot() override;

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

        bool OnNodeCreate(Node& newEditorNode, StringId32 typeId) override;
        void OnNodeDelete(Node& nodeToDelete) override;
        void OnLinkCreate(const Link& newLink) override;
        void OnLinkDelete(const Link& linkToDelete) override;

    protected:
        NodeListContextMenuItem m_ContextMenuRoot;
    };

    template <typename GraphAssetT, typename NodeFactoryT>
    class TypedNodeGraphEditorContext : public NodeGraphEditorContext
    {
    public:
        TypedNodeGraphEditorContext()
        {
            Graph = Reference<GraphAssetT>::Create();
        }

        DynamicArray<StringView> GetExtensions() const override { return Assets::AssetSystem::GetExtensions<GraphAssetT>(); }

    protected:
        NodeGraph::NodeGraph& GetNodeGraph() override { return Graph->GetNodeGraph(); }
        const NodeGraph::NodeGraph& GetNodeGraph() const override { return Graph->GetNodeGraph(); }

    private:
        const NodeGraph::INodeFactory& GetNodeFactory() const override { return NodeFactory; }
        void OnLoad(Assets::AssetSystem& assetSystem, const FileSystem::Filepath& path) override
        {
            const Assets::AssetId assetId(path);
            // want to queue meta data loading
            m_CurrentAssetMetaData = assetSystem.GetAssetMeta(assetId);

            Reference<GraphAssetT> graphAsset;
            assetSystem.GetAssetUnmanaged(assetId, graphAsset);
            if (graphAsset.IsValid() && graphAsset->IsLoaded())
            {
                Reference<typename GraphAssetT::AssetT> baseAsset(graphAsset);
                OnAssetLoaded(baseAsset);
            }
            else
            {
                graphAsset->GetOnLoadedEvent().template Connect<&TypedNodeGraphEditorContext::OnAssetLoaded>(this);
            }
        }

        void OnSave(Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& metaData) override
        {
            assetSystem.SaveAssetAs(metaData.Path, Graph);
        }

        void OnAssetLoaded(Reference<typename GraphAssetT::AssetT>& loadedGraph)
        {
            Clear();
            Graph = loadedGraph;

            const auto& graphNodes = Graph->GetNodeGraph().GetNodes();

            DynamicArray<Node>& nodes = GetNodes();
            nodes.reserve(graphNodes.size());

            for (auto&& [id, nodeContainer] : graphNodes)
            {
                const UniquePtr<NodeGraph::Node>& node = nodeContainer.m_Data;
                Node& nodeEditorMeta = nodes.emplace_back(node->GetId(), node->GetName());
                nodeEditorMeta.LocalId = id;

                UpdateEditorNodeData(nodeEditorMeta, *node);
            }

            FinishLoading(m_CurrentAssetMetaData);
        }

    protected:
        Reference<GraphAssetT> Graph;
        NodeFactoryT NodeFactory;
        // TODO: Not ideal to save asset meta data here
        Assets::AssetMetaData m_CurrentAssetMetaData;
    };
}
