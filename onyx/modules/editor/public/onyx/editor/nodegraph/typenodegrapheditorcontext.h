#pragma once

#include <onyx/editor/nodegraph/nodegrapheditorcontext.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::Editor
{
    template <typename GraphAssetT, typename NodeFactoryT>
    class TypedNodeGraphEditorContext : public NodeGraphEditorContext
    {
    public:
        TypedNodeGraphEditorContext()
        {
            //Graph = Assets::AssetSystem::Create<GraphAssetT>();
        }

        DynamicArray<StringView> GetExtensions() const override { return Assets::AssetSystem::GetExtensions<GraphAssetT>(); }
        StringView GetLocalizedAssetTypeName() const override { return GetLocalizationModule().TryGetLocalized(GraphAssetT::TypeId).value_or("Unknown"); }

    protected:
        NodeGraph::NodeGraph& GetNodeGraph() override { return Graph->GetNodeGraph(); }
        const NodeGraph::NodeGraph& GetNodeGraph() const override { return Graph->GetNodeGraph(); }

    private:
        const NodeGraph::INodeFactory& GetNodeFactory() const override { return NodeFactory; }
        void OnLoad(Assets::AssetSystem& assetSystem, const FilePath& path) override
        {
            const Assets::AssetId assetId(path);
            // want to queue meta data loading
            m_CurrentAssetMetaData = assetSystem.GetAssetMeta(assetId);

            Assets::AssetHandle<GraphAssetT> graphAsset;
            assetSystem.GetAssetUnmanaged(assetId, graphAsset);
            if (graphAsset.IsValid() && graphAsset->IsLoaded())
            {
                Assets::AssetHandle<typename GraphAssetT::AssetT> baseAsset(graphAsset);
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

        void OnAssetLoaded(Assets::AssetHandle<typename GraphAssetT::AssetT> loadedGraph)
        {
            Clear();
            Graph = loadedGraph;

            const auto& graphNodes = Graph->GetNodeGraph().GetNodes();

            DynamicArray<Node>& nodes = GetNodes();
            nodes.reserve(graphNodes.size());

            for (auto&& [id, nodeContainer] : graphNodes)
            {
                const UniquePtr<NodeGraph::Node>& node = nodeContainer.m_Data;

                String nodeName(node->GetName());
                if (nodeName.empty())
                {
                    nodeName = GetLocalizationModule().GetLocalized(node->GetTypeId()).Get();
                }

                Node& nodeEditorMeta = nodes.emplace_back(node->GetId(), nodeName);
                nodeEditorMeta.LocalId = id;

                UpdateEditorNodeData(nodeEditorMeta, *node);
            }

            FinishLoading(m_CurrentAssetMetaData);
        }

    protected:
        Assets::AssetHandle<GraphAssetT> Graph;
        NodeFactoryT NodeFactory;
        // TODO: Not ideal to save asset meta data here
        Assets::AssetMetaData m_CurrentAssetMetaData;
    };
}
