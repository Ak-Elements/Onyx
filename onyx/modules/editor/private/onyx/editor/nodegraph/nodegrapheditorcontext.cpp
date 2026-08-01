#include <onyx/editor/nodegraph/nodegrapheditorcontext.h>

#include <onyx/editor/windows/nodegrapheditor.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>
#include <onyx/thread/threadpool/threadpool.h>

namespace onyx::editor {
struct DefaultNodeFilter {
    bool operator()( node_graph::NodeEditorMetaData& ) const { return true; }
};

void NodeGraphEditorContext::onDrawNode( const Node& node ) {
    node_graph::Node& nodeGraphNode = getNodeGraphNode( node.LocalId );
    nodeGraphNode.uiDrawNode();
}

void NodeGraphEditorContext::onDrawNodeBackground( const Node& node ) {
    node_graph::Node& nodeGraphNode = getNodeGraphNode( node.LocalId );
    nodeGraphNode.uiDrawNodeBackground();
}

bool NodeGraphEditorContext::isNewLinkValid( Guid64 fromPinId, Guid64 toPinId ) const {
    return getNodeGraph().isNewLinkValid( fromPinId, toPinId ) == false;
}

bool NodeGraphEditorContext::arePinTypesCompatible( node_graph::PinTypeId lhsPinType,
                                                    node_graph::PinTypeId rhsPinType ) const {
    return lhsPinType == rhsPinType;
}

void NodeGraphEditorContext::drawNodeInPropertyPanel( Guid64 nodeId ) {
    Node& editorNode = getNode( nodeId );
    node_graph::Node& graphNode = getNodeGraphNode( editorNode.LocalId );
    HashMap< Guid64, std::any >& constantPinData = getNodeGraph().getConstantPinData();
    // TODO: Change bool to enum - NodeChanged::PinAdded, NodeChanged::PinRemoved NodeChanged::PinRenamed etc...
    bool hasChanged = graphNode.drawInPropertyGrid( constantPinData );

    if( hasChanged ) {
        updateEditorNodeData( editorNode, graphNode );
    }
}

void NodeGraphEditorContext::filterNodeListContextMenu(
    InplaceFunction< bool( StringView, const node_graph::NodeEditorMetaData& ) > filterFunctor ) {
    m_contextMenuRoot.Children.clear();

    const node_graph::INodeFactory& factory = getNodeFactory();
    const auto& nodeTypeIds = factory.getRegisteredNodeIds();
    const localization::LocalizationModule& localizationModule = getLocalizationModule();

    for( const StringId32 typeId : nodeTypeIds ) {
        const node_graph::NodeEditorMetaData& nodeMetaData = factory.getNodeMetaData( typeId );
        StringView localizedFullyQualifiedNodeName = localizationModule.getLocalized( nodeMetaData.TypeId ).Get();
        if( filterFunctor && filterFunctor( localizedFullyQualifiedNodeName, nodeMetaData ) == false ) {
            continue;
        }

        constexpr char delimiter = '/';
        DynamicArray< String > parts = split( localizedFullyQualifiedNodeName, delimiter );

        ui::TreeItem* currentParent = &m_contextMenuRoot;
        for( uint32_t i = 0; i < parts.size(); ++i ) {
            const String& currentToken = parts[ i ];
            if( i == parts.size() - 1 ) {
                ui::TreeItem& menuItem = currentParent->Children[ currentToken ];
                menuItem.Label = currentToken;
                menuItem.OnSelected = [ &, typeId ]() { createNewNode( typeId ); };

                break;
            }

            currentParent = &currentParent->Children[ currentToken ];
            currentParent->Label = currentToken;
        }
    }
}

void NodeGraphEditorContext::clearNodeListFilter() {
    filterNodeListContextMenu( nullptr );
}

const ui::TreeItem& NodeGraphEditorContext::getNodeListContextMenuRoot() {
    return m_contextMenuRoot;
}

void NodeGraphEditorContext::updateEditorNodeData( Node& editorNode, const node_graph::Node& graphNode ) {
    DynamicArray< Link >& editorLinks = getLinks();
    const uint32_t inputPinCount = graphNode.getInputPinCount();
    editorNode.Inputs.clear();
    editorNode.Inputs.reserve( inputPinCount );
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = graphNode.getInputPin( i );

        Pin& inputPinEditorMeta = editorNode.Inputs.emplace_back();
        inputPinEditorMeta.Name = graphNode.getPinName( inputPin->getLocalId() );
        inputPinEditorMeta.Id = inputPin->getGlobalId();
        inputPinEditorMeta.LocalId = inputPin->getLocalId();
        inputPinEditorMeta.PinTypeId = inputPin->getType();
        inputPinEditorMeta.Color = inputPin->getTypeColor();
        inputPinEditorMeta.Direction = PinDirection::Input;

        if( inputPin->isConnected() ) {
            editorLinks.emplace_back( Guid64Generator::getGuid(),
                                      inputPin->getGlobalId(),
                                      inputPin->getLinkedPinGlobalId(),
                                      graphNode.getId(),
                                      getNodeGraph().getNodeForPinId( inputPin->getLinkedPinGlobalId() ).getId(),
                                      inputPin->getTypeColor() );
        }
    }

    const uint32_t outputPinCount = graphNode.getOutputPinCount();
    editorNode.Outputs.clear();
    editorNode.Outputs.reserve( outputPinCount );
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = graphNode.getOutputPin( i );

        Pin& outputPinEditorMeta = editorNode.Outputs.emplace_back();
        outputPinEditorMeta.Name = graphNode.getPinName( outputPin->getLocalId() );
        outputPinEditorMeta.Id = outputPin->getGlobalId();
        outputPinEditorMeta.LocalId = outputPin->getLocalId();
        outputPinEditorMeta.PinTypeId = outputPin->getType();
        outputPinEditorMeta.Color = outputPin->getTypeColor();
        outputPinEditorMeta.Direction = PinDirection::Output;

        if( outputPin->isConnected() ) {
            editorLinks.emplace_back( Guid64Generator::getGuid(),
                                      outputPin->getGlobalId(),
                                      outputPin->getLinkedPinGlobalId(),
                                      graphNode.getId(),
                                      getNodeGraph().getNodeForPinId( outputPin->getLinkedPinGlobalId() ).getId(),
                                      outputPin->getTypeColor() );
        }
    }
}

void NodeGraphEditorContext::onNodeDelete( Node& nodeToDelete ) {
    getNodeGraph().remove( nodeToDelete.LocalId );
}

void NodeGraphEditorContext::onLinkCreate( const Link& newLink ) {
    node_graph::NodeGraph& graph = getNodeGraph();
    node_graph::Node& node = graph.getNodeForPinId( newLink.FromPinId );

    const uint32_t inputPinCount = node.getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        node_graph::PinBase* inputPin = node.getInputPin( i );
        if( inputPin->getGlobalId() != newLink.FromPinId )
            continue;

        // disconnect the connection
        if( inputPin->isConnected() ) {
            deleteLink( newLink.FromPinId, inputPin->getLinkedPinGlobalId() );
        }

        graph.addEdge( newLink.ToPinId, newLink.FromPinId );
        inputPin->connectPin( newLink.ToPinId );
        return;
    }

    const uint32_t outputPinCount = node.getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        node_graph::PinBase* outputPin = node.getOutputPin( i );
        if( outputPin->getGlobalId() != newLink.FromPinId )
            continue;

        // disconnect the connection
        if( outputPin->isConnected() ) {
            deleteLink( newLink.FromPinId, outputPin->getLinkedPinGlobalId() );
        }

        graph.addEdge( newLink.FromPinId, newLink.ToPinId );
        outputPin->connectPin( newLink.ToPinId );
        return;
    }

    ONYX_ASSERT( false, "Failed creating link" );
}

void NodeGraphEditorContext::onLinkDelete( const Link& link ) {
    node_graph::Node& node = getNodeGraph().getNodeForPinId( link.FromPinId );

    const uint32_t inputPinCount = node.getInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        node_graph::PinBase* inputPin = node.getInputPin( i );
        if( inputPin->getGlobalId() != link.FromPinId )
            continue;

        inputPin->clearLink();
        return;
    }

    const uint32_t outputPinCount = node.getOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        node_graph::PinBase* outputPin = node.getOutputPin( i );
        if( outputPin->getGlobalId() != link.FromPinId )
            continue;

        outputPin->clearLink();
        return;
    }

    ONYX_ASSERT( false, "Failed clearing link from graph" );
}

bool NodeGraphEditorContext::onNodeCreate( Node& newEditorNode, StringId32 typeId ) {
    UniquePtr< node_graph::Node > newNode = getNodeFactory().createNode( typeId );
    newNode->setId( newEditorNode.Id );

    newEditorNode.Name = newNode->getName();
    if( newEditorNode.Name.empty() ) {
        StringView localizedFullyQualifiedNodeName = getLocalizationModule().getLocalized( typeId ).Get();
        constexpr char delimiter = '/';
        DynamicArray< String > parts = split( localizedFullyQualifiedNodeName, delimiter );
        newEditorNode.Name = parts.back();
    }

    updateEditorNodeData( newEditorNode, *newNode );

    const node_graph::NodeEditorMetaData& nodeMetaData = getNodeFactory().getNodeMetaData( typeId );
    newEditorNode.ShowNodeName = nodeMetaData.ShowNodeName;
    newEditorNode.LocalId = getNodeGraph().emplace( std::move( newNode ) );
    return true;
}
} // namespace onyx::editor
