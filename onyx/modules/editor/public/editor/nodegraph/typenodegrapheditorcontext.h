#pragma once

#include <editor/nodegraph/nodegrapheditorcontext.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodefactory.h>

namespace Onyx::Editor
{
    template <typename GraphAssetT, typename NodeFactoryT>
    class TypedNodeGraphEditorContext : public NodeGraphEditorContext
    {
    public:
        TypedNodeGraphEditorContext()
        {
            Graph = Reference<GraphAssetT>::Create();
        }

    protected:
        NodeGraph::NodeGraph& GetGraph() override { return Graph->GetGraph(); }
        const NodeGraph::NodeGraph& GetGraph() const override { return Graph->GetGraph(); }

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
                OnAssetLoaded(graphAsset);
            }
            else
            {
                graphAsset->GetOnLoadedEvent().Connect<&TypedNodeGraphEditorContext::OnAssetLoaded>(this);
            }
        }

        void OnSave(Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& /*metaData*/) override
        {
            assetSystem.SaveAsset<GraphAssetT>(Graph->GetId());
        }

        DynamicArray<StringView> GetExtensions() const override
        {
            DynamicArray<StringView> extensions;
            return extensions;
        }

        void OnAssetLoaded(Reference<GraphAssetT>& loadedGraph)
        {
            Clear();
            Graph = loadedGraph;

            const auto& graphNodes = Graph->GetGraph().GetNodes();

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
