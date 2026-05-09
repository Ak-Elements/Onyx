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
    nodeGraphNode.UIDrawNode();
}

void NodeGraphEditorContext::onDrawNodeBackground( const Node& node ) {
    node_graph::Node& nodeGraphNode = getNodeGraphNode( node.LocalId );
    nodeGraphNode.UIDrawNodeBackground();
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
    bool hasChanged = graphNode.DrawInPropertyGrid( constantPinData );

    if( hasChanged ) {
        updateEditorNodeData( editorNode, graphNode );
    }
}

void NodeGraphEditorContext::filterNodeListContextMenu(
    InplaceFunction< bool( StringView, const node_graph::NodeEditorMetaData& ) > filterFunctor ) {
    m_contextMenuRoot.Children.clear();

    const node_graph::INodeFactory& factory = getNodeFactory();
    const auto& nodeTypeIds = factory.GetRegisteredNodeIds();
    const localization::LocalizationModule& localizationModule = getLocalizationModule();

    for( const StringId32 typeId : nodeTypeIds ) {
        const node_graph::NodeEditorMetaData& nodeMetaData = factory.GetNodeMetaData( typeId );
        StringView localizedFullyQualifiedNodeName = localizationModule.GetLocalized( nodeMetaData.TypeId ).Get();
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
    const uint32_t inputPinCount = graphNode.GetInputPinCount();
    editorNode.Inputs.clear();
    editorNode.Inputs.reserve( inputPinCount );
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        const node_graph::PinBase* inputPin = graphNode.GetInputPin( i );

        Pin& inputPinEditorMeta = editorNode.Inputs.emplace_back();
        inputPinEditorMeta.Name = graphNode.GetPinName( inputPin->GetLocalId() );
        inputPinEditorMeta.Id = inputPin->GetGlobalId();
        inputPinEditorMeta.LocalId = inputPin->GetLocalId();
        inputPinEditorMeta.PinTypeId = inputPin->GetType();
        inputPinEditorMeta.Color = inputPin->GetTypeColor();
        inputPinEditorMeta.Direction = PinDirection::Input;

        if( inputPin->IsConnected() ) {
            editorLinks.emplace_back( Guid64Generator::getGuid(),
                                      inputPin->GetGlobalId(),
                                      inputPin->GetLinkedPinGlobalId(),
                                      graphNode.GetId(),
                                      getNodeGraph().getNodeForPinId( inputPin->GetLinkedPinGlobalId() ).GetId(),
                                      inputPin->GetTypeColor() );
        }
    }

    const uint32_t outputPinCount = graphNode.GetOutputPinCount();
    editorNode.Outputs.clear();
    editorNode.Outputs.reserve( outputPinCount );
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        const node_graph::PinBase* outputPin = graphNode.GetOutputPin( i );

        Pin& outputPinEditorMeta = editorNode.Outputs.emplace_back();
        outputPinEditorMeta.Name = graphNode.GetPinName( outputPin->GetLocalId() );
        outputPinEditorMeta.Id = outputPin->GetGlobalId();
        outputPinEditorMeta.LocalId = outputPin->GetLocalId();
        outputPinEditorMeta.PinTypeId = outputPin->GetType();
        outputPinEditorMeta.Color = outputPin->GetTypeColor();
        outputPinEditorMeta.Direction = PinDirection::Output;

        if( outputPin->IsConnected() ) {
            editorLinks.emplace_back( Guid64Generator::getGuid(),
                                      outputPin->GetGlobalId(),
                                      outputPin->GetLinkedPinGlobalId(),
                                      graphNode.GetId(),
                                      getNodeGraph().getNodeForPinId( outputPin->GetLinkedPinGlobalId() ).GetId(),
                                      outputPin->GetTypeColor() );
        }
    }
}

void NodeGraphEditorContext::onNodeDelete( Node& nodeToDelete ) {
    getNodeGraph().remove( nodeToDelete.LocalId );
}

void NodeGraphEditorContext::onLinkCreate( const Link& newLink ) {
    node_graph::NodeGraph& graph = getNodeGraph();
    node_graph::Node& node = graph.getNodeForPinId( newLink.FromPinId );

    const uint32_t inputPinCount = node.GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        node_graph::PinBase* inputPin = node.GetInputPin( i );
        if( inputPin->GetGlobalId() != newLink.FromPinId )
            continue;

        // disconnect the connection
        if( inputPin->IsConnected() ) {
            deleteLink( newLink.FromPinId, inputPin->GetLinkedPinGlobalId() );
        }

        graph.addEdge( newLink.ToPinId, newLink.FromPinId );
        inputPin->ConnectPin( newLink.ToPinId );
        return;
    }

    const uint32_t outputPinCount = node.GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        node_graph::PinBase* outputPin = node.GetOutputPin( i );
        if( outputPin->GetGlobalId() != newLink.FromPinId )
            continue;

        // disconnect the connection
        if( outputPin->IsConnected() ) {
            deleteLink( newLink.FromPinId, outputPin->GetLinkedPinGlobalId() );
        }

        graph.addEdge( newLink.FromPinId, newLink.ToPinId );
        outputPin->ConnectPin( newLink.ToPinId );
        return;
    }

    ONYX_ASSERT( false, "Failed creating link" );
}

void NodeGraphEditorContext::onLinkDelete( const Link& link ) {
    node_graph::Node& node = getNodeGraph().getNodeForPinId( link.FromPinId );

    const uint32_t inputPinCount = node.GetInputPinCount();
    for( uint32_t i = 0; i < inputPinCount; ++i ) {
        node_graph::PinBase* inputPin = node.GetInputPin( i );
        if( inputPin->GetGlobalId() != link.FromPinId )
            continue;

        inputPin->ClearLink();
        return;
    }

    const uint32_t outputPinCount = node.GetOutputPinCount();
    for( uint32_t i = 0; i < outputPinCount; ++i ) {
        node_graph::PinBase* outputPin = node.GetOutputPin( i );
        if( outputPin->GetGlobalId() != link.FromPinId )
            continue;

        outputPin->ClearLink();
        return;
    }

    ONYX_ASSERT( false, "Failed clearing link from graph" );
}

bool NodeGraphEditorContext::onNodeCreate( Node& newEditorNode, StringId32 typeId ) {
    UniquePtr< node_graph::Node > newNode = getNodeFactory().CreateNode( typeId );
    newNode->SetId( newEditorNode.Id );

    newEditorNode.Name = newNode->GetName();
    if( newEditorNode.Name.empty() ) {
        StringView localizedFullyQualifiedNodeName = getLocalizationModule().GetLocalized( typeId ).Get();
        constexpr char delimiter = '/';
        DynamicArray< String > parts = split( localizedFullyQualifiedNodeName, delimiter );
        newEditorNode.Name = parts.back();
    }

    updateEditorNodeData( newEditorNode, *newNode );

    const node_graph::NodeEditorMetaData& nodeMetaData = getNodeFactory().GetNodeMetaData( typeId );
    newEditorNode.ShowNodeName = nodeMetaData.ShowNodeName;
    newEditorNode.LocalId = getNodeGraph().emplace( std::move( newNode ) );
    return true;
}
} // namespace onyx::editor
