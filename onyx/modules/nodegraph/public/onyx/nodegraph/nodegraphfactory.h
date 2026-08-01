#pragma once

#include <onyx/nodegraph/nodes/node.h>

namespace onyx::localization {
struct LocalizationId;
}

namespace onyx::file_system {
struct JsonValue;
}

namespace onyx::node_graph {
class Node;
struct ExecutionContext;

namespace details {
template < typename T >
concept IsNodeGraphNode = requires( T& node, ExecutionContext& context, Guid64 globalPinId ) {
    //{ node.Prepare() };
    //{ node.Update(context) };
    //{ node.Finish() };
    { node.hasPin( globalPinId ) };
} && HasTypeId< T >;

template < typename T >
concept HasAliases = requires( T node ) {
    { T::HasAliases };
};

template < typename T >
concept HasShowNodeName = requires( T node ) {
    { T::ShowNodeName };
};
} // namespace details

enum class GraphContext : uint8_t {
    Invalid,
    NodeGraph,
    ShaderGraph,
};

struct NodeEditorMetaData {
    DynamicArray< PinTypeId > InputPins;
    DynamicArray< PinTypeId > OutputPins;

    StringId32 TypeId;
    bool HasAliases;
    bool ShowNodeName;
};

class INodeFactory {
  public:
    virtual ~INodeFactory() = default;
    [[nodiscard]] virtual UniquePtr< Node > createNode( StringId32 typeId ) const = 0;
    [[nodiscard]] virtual const HashSet< StringId32 >& getRegisteredNodeIds() const = 0;
    [[nodiscard]] virtual const NodeEditorMetaData& getNodeMetaData( StringId32 typeId ) const = 0;
};

template < typename MetaDataContainerT = NodeEditorMetaData >
requires std::is_base_of_v< NodeEditorMetaData, MetaDataContainerT >
class NodeRegistry {
    // serialize / deserialize functors
  public:
    UniquePtr< Node > createNode( StringId32 typeId ) {
        ONYX_ASSERT( m_registeredNodes.contains( typeId ), "Node is not registered in this context" );
        return UniquePtr< Node >( m_registeredNodes.at( typeId )() ); // call functor to create new node
    }

    template < details::IsNodeGraphNode NodeT >
    void registerNode() {
        constexpr StringId32 TypeId = NodeT::TypeId;
        ONYX_ASSERT( m_registeredNodeTypeIds.contains( TypeId ) == false,
                     "Node is already registered in this context" );

        m_registeredNodeTypeIds.emplace( TypeId );
        MetaDataContainerT& metaContainer = m_registeredNodesMetaData[ TypeId ];
        metaContainer.TypeId = TypeId;
        metaContainer.HasAliases = details::HasAliases< NodeT >;
        if constexpr( details::HasShowNodeName< NodeT > ) {
            metaContainer.ShowNodeName = NodeT::ShowNodeName;
        } else {
            metaContainer.ShowNodeName = true;
        }

        // can we make this constexpr?
        NodeT node{};
        uint32_t inputPinCount = node.getInputPinCount();
        for( uint32_t i = 0; i < inputPinCount; ++i ) {
            metaContainer.InputPins.push_back( node.getInputPin( i )->getType() );
        }

        uint32_t outputPinCount = node.getOutputPinCount();
        for( uint32_t i = 0; i < outputPinCount; ++i ) {
            metaContainer.OutputPins.push_back( node.getOutputPin( i )->getType() );
        }

        m_registeredNodes[ TypeId ] = [ = ]() {
            NodeT* newNode = new NodeT();
            return newNode;
        };
    }

    MetaDataContainerT& getNodeMetaData( StringId32 typeId ) {
        ONYX_ASSERT( m_registeredNodesMetaData.contains( typeId ), "Node with that ID is not registered." );
        return m_registeredNodesMetaData.at( typeId );
    }

    const HashMap< StringId32, MetaDataContainerT >& getRegisteredNodesMetaData() { return m_registeredNodesMetaData; }
    const HashSet< StringId32 >& getRegisteredNodeIds() const { return m_registeredNodeTypeIds; }

  protected:
    HashSet< StringId32 > m_registeredNodeTypeIds;
    HashMap< StringId32, InplaceFunction< Node*() > > m_registeredNodes;
    HashMap< StringId32, MetaDataContainerT > m_registeredNodesMetaData;
};

template < typename NodeType, typename MetaDataType >
class TypedNodeFactory : public INodeFactory {
  public:
    using NodeTypeT = NodeType;

    template < details::IsNodeGraphNode T >
    static void registerNode() {
        s_msNodeRegistry.template registerNode< T >();
    }

    [[nodiscard]] UniquePtr< Node > createNode( StringId32 typeHash ) const override {
        return s_msNodeRegistry.createNode( typeHash );
    }

    [[nodiscard]] const NodeEditorMetaData& getNodeMetaData( StringId32 typeHash ) const override {
        return s_msNodeRegistry.getNodeMetaData( typeHash );
    }

    [[nodiscard]] const HashSet< StringId32 >& getRegisteredNodeIds() const override {
        return s_msNodeRegistry.getRegisteredNodeIds();
    }

  protected:
    static NodeRegistry< MetaDataType > s_msNodeRegistry;
};

template < typename NodeType, typename MetaDataType >
NodeRegistry< MetaDataType > TypedNodeFactory< NodeType, MetaDataType >::s_msNodeRegistry;

class NodeGraphFactory : public TypedNodeFactory< Node, NodeEditorMetaData > {
  public:
    // TODO: Node concept to enforce node
    template < typename T >
    static void registerNode() {
        TypedNodeFactory::registerNode< T >();
    }
};
} // namespace onyx::node_graph
