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
    DynamicArray< StringView > getExtensions() const override {
        return assets::AssetSystem::getExtensions< GraphAssetT >();
    }
    StringView getLocalizedAssetTypeName() const override {
        const auto& localizationSystem = getEngineSystem< localization::LocalizationModule >();
        return localizationSystem.tryGetLocalized( GraphAssetT::TypeId ).value_or( "Unknown" );
    }

    void createNew() override {
        assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
        m_graph = assetSystem.create< GraphAssetT >();
    }

  protected:
    node_graph::NodeGraph& getNodeGraph() override { return m_graph->getNodeGraph(); }
    const node_graph::NodeGraph& getNodeGraph() const override { return m_graph->getNodeGraph(); }

  private:
    const node_graph::INodeFactory& getNodeFactory() const override { return m_nodeFactory; }
    void onLoad( const FilePath& path ) override {
        assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
        const assets::AssetId assetId = assetSystem.resolveAssetId( path );

        assets::AssetHandle< GraphAssetT > graphAsset;
        assetSystem.getAssetUnmanaged( assetId, graphAsset );
        if( graphAsset.isValid() && graphAsset->isLoaded() ) {
            assets::AssetHandle< typename GraphAssetT::AssetT > baseAsset( graphAsset );
            onAssetLoaded( baseAsset );
        } else {
            graphAsset->getOnLoadedEvent().template connect< &TypedNodeGraphEditorContext::onAssetLoaded >( this );
        }
    }

    void onSave() override {
        assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
        assetSystem.saveAsset( m_graph );
        m_graph->getOnSavedEvent().template connect< &TypedNodeGraphEditorContext::onAssetSaved >( this );
    }

    void onSave( const FilePath& path ) override {
        assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
        m_graph = assetSystem.saveAssetAs( path, m_graph );
        m_graph->getOnSavedEvent().template connect< &TypedNodeGraphEditorContext::onAssetSaved >( this );
    }

    void onClear() override { m_graph.reset(); }

    void onAssetLoaded( assets::AssetHandle< typename GraphAssetT::AssetT > loadedGraph ) {
        clear();

        loadedGraph->getOnLoadedEvent().disconnect( this );
        m_graph = loadedGraph;

        const auto& graphNodes = m_graph->getNodeGraph().getNodes();

        DynamicArray< Node >& nodes = getNodes();
        nodes.reserve( graphNodes.size() );

        const auto& localizationSystem = getEngineSystem< localization::LocalizationModule >();
        for( auto&& [ id, nodeContainer ] : graphNodes ) {
            const UniquePtr< node_graph::Node >& node = nodeContainer.Data;

            String nodeName( node->getName() );
            if( nodeName.empty() ) {
                nodeName = localizationSystem.getLocalized( node->getTypeId() ).Get();
            }

            Node& nodeEditorMeta = nodes.emplace_back( node->getId(), nodeName );
            nodeEditorMeta.LocalId = id;

            updateEditorNodeData( nodeEditorMeta, *node );
        }

        onLoaded( m_graph.getId() );
    }

    void onAssetSaved( assets::AssetHandle< typename GraphAssetT::AssetT > savedGraph, bool savedSuccessfully ) {
        m_graph->getOnSavedEvent().disconnect( this );
        if( savedSuccessfully ) {
            onSaved( savedGraph.getId() );
        } else {
            const assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
            const assets::AssetMetaData& assetMeta = assetSystem.getAssetMeta( savedGraph.getId() );
            ONYX_LOG_WARNING( "Failed saving {} at path {}", getLocalizedAssetTypeName(), assetMeta.Path );
        }
    }

  protected:
    assets::AssetHandle< GraphAssetT > m_graph;
    NodeFactoryT m_nodeFactory;
};
} // namespace onyx::editor
