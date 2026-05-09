#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/editor/nodegraph/grapheditorcontext.h>

namespace onyx::editor {
void GraphEditorContext::setLocalizationModule( const localization::LocalizationModule& localizationModule ) {
    m_localizationModule = &localizationModule;
}

const localization::LocalizationModule& GraphEditorContext::getLocalizationModule() const {
    ONYX_ASSERT( m_localizationModule != nullptr );
    return *m_localizationModule;
}

void GraphEditorContext::clear() {
    m_nodes.clear();
    m_links.clear();
}

GraphEditorContext::Node& GraphEditorContext::createNewNode( StringId32 nodeTypeId ) {
    Guid64 newNodeId = Guid64Generator::getGuid();
    Node tmpNode( newNodeId );
    bool hasSucceeded = onNodeCreate( tmpNode, nodeTypeId );

    if( hasSucceeded == false ) {
        ONYX_ASSERT( false, "Failed creating node" );
        return m_nodes.back();
    }

    Node& newNode = m_nodes.emplace_back( std::move( tmpNode ) );
    if( OnNodeCreated ) {
        OnNodeCreated( newNode );
    }

    return newNode;
}

void GraphEditorContext::deleteNode( Guid64 nodeId ) {
    auto it = std::ranges::find_if( m_nodes, [ & ]( const Node& node ) { return node.Id == nodeId; } );

    if( it == m_nodes.end() )
        return;

    for( auto linksIt = m_links.begin(); linksIt != m_links.end(); ) {
        const Link& link = *linksIt;
        if( link.FromNodeId == nodeId ) {
            onLinkDelete( link );
            linksIt = m_links.erase( linksIt );
            continue;
        }

        if( link.ToNodeId == nodeId ) {
            onLinkDelete( link );
            linksIt = m_links.erase( linksIt );
            continue;
        }

        ++linksIt;
    }

    Node& editorNode = *it;
    onNodeDelete( editorNode );

    if( OnNodeDeleted ) {
        OnNodeDeleted( editorNode );
    }

    m_nodes.erase( it );
}

GraphEditorContext::Node& GraphEditorContext::getNode( Guid64 nodeId ) {
    auto it = std::ranges::find_if( m_nodes, [ & ]( Node& node ) { return node.Id == nodeId; } );

    return *it;
}

const GraphEditorContext::Node& GraphEditorContext::getNode( Guid64 nodeId ) const {
    auto it = std::find_if( m_nodes.begin(), m_nodes.end(), [ & ]( const Node& node ) { return node.Id == nodeId; } );

    return *it;
}

GraphEditorContext::Node& GraphEditorContext::getNodeForPin( Guid64 pinId ) {
    for( Node& node : m_nodes ) {
        for( Pin& inputPin : node.Inputs ) {
            if( inputPin.Id == pinId )
                return node;
        }

        for( Pin& outputPin : node.Outputs ) {
            if( outputPin.Id == pinId )
                return node;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with ID {}", pinId.get() );
    return m_nodes[ 0 ];
}

const GraphEditorContext::Node& GraphEditorContext::getNodeForPin( Guid64 pinId ) const {
    for( const Node& node : m_nodes ) {
        for( const Pin& inputPin : node.Inputs ) {
            if( inputPin.Id == pinId )
                return node;
        }

        for( const Pin& outputPin : node.Outputs ) {
            if( outputPin.Id == pinId )
                return node;
        }
    }

    ONYX_ASSERT( false, "Failed finding pin with ID {}", pinId.get() );
    return m_nodes[ 0 ];
}

void GraphEditorContext::setNodeName( Guid64 nodeId, const String& name ) {
    Node& node = getNode( nodeId );
    node.Name = name;
}

void GraphEditorContext::setNodePosition( Guid64 nodeId, const Vector2f32& position ) {
    Node& node = getNode( nodeId );

    if( node.Position != position ) {
        node.Position = position;
        node.HasUpdatedPosition = true;
    }
}

GraphEditorContext::Link& GraphEditorContext::createNewLink( Guid64 fromPinId, Guid64 toPinId ) {
    Link newLink( Guid64Generator::getGuid(),
                  fromPinId,
                  toPinId,
                  getNodeForPin( fromPinId ).Id,
                  getNodeForPin( toPinId ).Id,
                  getPin( fromPinId ).Color );

    onLinkCreate( newLink );

    if( OnLinkCreated ) {
        OnLinkCreated( newLink );
    }

    return m_links.emplace_back( newLink );
}

void GraphEditorContext::deleteLink( Guid64 linkId ) {
    auto it = std::find_if( m_links.begin(), m_links.end(), [ & ]( const Link& link ) { return link.Id == linkId; } );

    if( it == m_links.end() )
        return;

    const Link& link = *it;

    onLinkDelete( link );

    if( OnLinkDeleted ) {
        OnLinkDeleted( link );
    }

    m_links.erase( it );
}

void GraphEditorContext::deleteLink( Guid64 fromPinId, Guid64 toPinId ) {
    auto it = std::find_if( m_links.begin(), m_links.end(), [ & ]( const Link& link ) {
        return ( link.FromPinId == fromPinId ) && ( link.ToPinId == toPinId );
    } );

    if( it == m_links.end() )
        return;

    const Link& link = *it;

    onLinkDelete( link );

    if( OnLinkDeleted ) {
        OnLinkDeleted( link );
    }

    m_links.erase( it );
}

GraphEditorContext::Pin& GraphEditorContext::getPin( Guid64 pinId ) {
    for( Node& node : m_nodes ) {
        for( Pin& inputPin : node.Inputs ) {
            if( inputPin.Id == pinId )
                return inputPin;
        }

        for( Pin& outputPin : node.Outputs ) {
            if( outputPin.Id == pinId )
                return outputPin;
        }
    }

    ONYX_ASSERT( false, "Failed getting pin for id" );
    return m_nodes[ 0 ].Inputs[ 0 ];
}

const GraphEditorContext::Pin& GraphEditorContext::getPin( Guid64 pinId ) const {
    for( const Node& node : m_nodes ) {
        for( const Pin& inputPin : node.Inputs ) {
            if( inputPin.Id == pinId )
                return inputPin;
        }

        for( const Pin& outputPin : node.Outputs ) {
            if( outputPin.Id == pinId )
                return outputPin;
        }
    }

    ONYX_ASSERT( false, "Failed getting pin for id" );
    return m_nodes[ 0 ].Inputs[ 0 ];
}

bool GraphEditorContext::hasPin( Guid64 pinId ) const {
    for( const Node& node : m_nodes ) {
        if( std::ranges::any_of( node.Inputs, [ & ]( const Pin& pin ) { return pin.Id == pinId; } ) ) {
            return true;
        }

        if( std::ranges::any_of( node.Outputs, [ & ]( const Pin& pin ) { return pin.Id == pinId; } ) ) {
            return true;
        }
    }

    return false;
}

bool GraphEditorContext::isPinLinked( Guid64 pindId ) const {
    return std::ranges::any_of( m_links, [ & ]( const Link& link ) {
        return ( link.FromPinId == pindId ) || ( link.ToPinId == pindId );
    } );
}

void GraphEditorContext::drawNode( const Node& node ) {
    onDrawNode( node );
}

void GraphEditorContext::drawNodeBackground( const Node& node ) {
    onDrawNodeBackground( node );
}

void GraphEditorContext::load( assets::AssetSystem& assetSystem, const FilePath& path ) {
    m_isLoading = true;
    onLoad( assetSystem, path );
}

void GraphEditorContext::save( assets::AssetSystem& assetSystem, const assets::AssetMetaData& assetMeta ) {
    onSave( assetSystem, assetMeta );

    if( SaveEditorMetaDataFunctor ) {
        SaveEditorMetaDataFunctor( assetMeta.Path );
    }

    OnSaved();
}

void GraphEditorContext::finishLoading( const assets::AssetMetaData& assetMeta ) {
    if( LoadEditorMetaDataFunctor ) {
        LoadEditorMetaDataFunctor( assetMeta.Path );
    }

    m_isLoading = false;
    OnLoaded();
}

void GraphEditorContext::onNodeChanged( const Node& /*newNode*/ ) {}
} // namespace onyx::editor
