#pragma once

#include <onyx/assets/assetsystem.h>
#include <onyx/editor/nodegraph/nodegrapheditorcontext.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>

namespace onyx::editor {
template < typename GraphAssetT, typename NodeFactoryT >
class TypedNodeGraphEditorContext : public NodeGraphEditorContext {
  public:
    TypedNodeGraphEditorContext();

    DynamicArray< StringView > getExtensions() const override {
        return assets::AssetSystem::getExtensions< GraphAssetT >();
    }
    StringView getLocalizedAssetTypeName() const override {
        return getLocalizationModule().TryGetLocalized( GraphAssetT::TypeId ).value_or( "Unknown" );
    }

  protected:
    node_graph::NodeGraph& getNodeGraph() override { return m_graph->getNodeGraph(); }
    const node_graph::NodeGraph& getNodeGraph() const override { return m_graph->getNodeGraph(); }

  private:
    const node_graph::INodeFactory& getNodeFactory() const override { return m_nodeFactory; }
    void onLoad( assets::AssetSystem& assetSystem, const FilePath& path ) override {
        const assets::AssetId assetId( path );
        // want to queue meta data loading
        m_currentAssetMetaData = assetSystem.getAssetMeta( assetId );

        assets::AssetHandle< GraphAssetT > graphAsset;
        assetSystem.getAssetUnmanaged( assetId, graphAsset );
        if( graphAsset.isValid() && graphAsset->isLoaded() ) {
            assets::AssetHandle< typename GraphAssetT::AssetT > baseAsset( graphAsset );
            onAssetLoaded( baseAsset );
        } else {
            graphAsset->getOnLoadedEvent().template Connect< &TypedNodeGraphEditorContext::onAssetLoaded >( this );
        }
    }

    void onSave( assets::AssetSystem& assetSystem, const assets::AssetMetaData& metaData ) override {
        assetSystem.saveAssetAs( metaData.Path, m_graph );
    }

    void onAssetLoaded( assets::AssetHandle< typename GraphAssetT::AssetT > loadedGraph ) {
        clear();
        m_graph = loadedGraph;

        const auto& graphNodes = m_graph->getNodeGraph().getNodes();

        DynamicArray< Node >& nodes = getNodes();
        nodes.reserve( graphNodes.size() );

        for( auto&& [ id, nodeContainer ] : graphNodes ) {
            const UniquePtr< node_graph::Node >& node = nodeContainer.Data;

            String nodeName( node->GetName() );
            if( nodeName.empty() ) {
                nodeName = getLocalizationModule().GetLocalized( node->GetTypeId() ).Get();
            }

            Node& nodeEditorMeta = nodes.emplace_back( node->GetId(), nodeName );
            nodeEditorMeta.LocalId = id;

            updateEditorNodeData( nodeEditorMeta, *node );
        }

        finishLoading( m_currentAssetMetaData );
    }

  protected:
    assets::AssetHandle< GraphAssetT > m_graph;
    NodeFactoryT m_nodeFactory;
    // TODO: Not ideal to save asset meta data here
    assets::AssetMetaData m_currentAssetMetaData;
};
template < typename GraphAssetT, typename NodeFactoryT >
inline TypedNodeGraphEditorContext< GraphAssetT, NodeFactoryT >::TypedNodeGraphEditorContext() = default;
} // namespace onyx::editor
