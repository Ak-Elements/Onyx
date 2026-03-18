#pragma once

#include <onyx/nodegraph/nodes/node.h>

namespace onyx::localization
{
    struct LocalizationId;
}

namespace onyx::file_system
{
    struct JsonValue;
}

namespace onyx::node_graph
{
    class Node;
    struct ExecutionContext;

    namespace Details
    {
        template <typename T>
        concept IsNodeGraphNode = requires(T& node, ExecutionContext& context, Guid64 globalPinId)
        {
            //{ node.Prepare() };
            //{ node.Update(context) };
            //{ node.Finish() };
            { node.HasPin(globalPinId) };
        } && HasTypeId<T>;

        template <typename T>
        concept HasAliases = requires(T node)
        {
            { T::HasAliases };
        };

        template <typename T>
        concept HasShowNodeName = requires(T node)
        {
            { T::ShowNodeName };
        };
    }

    enum class GraphContext : onyxU32
    {
        Invalid,
        NodeGraph,
        ShaderGraph,
    };

    struct NodeEditorMetaData
    {
        DynamicArray<PinTypeId> InputPins;
        DynamicArray<PinTypeId> OutputPins;

        StringId32 TypeId;
        bool HasAliases;
        bool ShowNodeName;
    };

    class INodeFactory
    { 
    public:
        virtual ~INodeFactory() = default;
        virtual UniquePtr<Node> CreateNode(StringId32 typeId) const = 0;
        virtual const HashSet<StringId32>& GetRegisteredNodeIds() const = 0;
        virtual const NodeEditorMetaData& GetNodeMetaData(StringId32 typeId) const = 0;
    };

    template <typename MetaDataContainerT = NodeEditorMetaData>
        requires std::is_base_of_v<NodeEditorMetaData, MetaDataContainerT>
    class NodeRegistry
    {
        // serialize / deserialize functors
    public:
        UniquePtr<Node> CreateNode(StringId32 typeId)
        {
            ONYX_ASSERT(m_RegisteredNodes.contains(typeId), "Node is not registered in this context");
            return UniquePtr<Node>(m_RegisteredNodes.at(typeId)()); // call functor to create new node
        }

        template <Details::IsNodeGraphNode NodeT>
        void Register()
        {
            static HashMap<onyxU32, String> s_RegisteredNodesToName;

            constexpr StringId32 typeId = NodeT::TypeId;
            ONYX_ASSERT(m_RegisteredNodeTypeIds.contains(typeId) == false, "Node is already registered in this context");

            m_RegisteredNodeTypeIds.emplace(typeId);
            MetaDataContainerT& metaContainer = m_RegisteredNodesMetaData[typeId];
            metaContainer.TypeId = typeId;
            metaContainer.HasAliases = Details::HasAliases<NodeT>;
            if constexpr (Details::HasShowNodeName<NodeT>)
            {
                metaContainer.ShowNodeName = NodeT::ShowNodeName;
            }
            else
            {
                metaContainer.ShowNodeName = true;
            }

            // can we make this constexpr?
            NodeT node{};
            onyxU32 inputPinCount = node.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                metaContainer.InputPins.push_back(node.GetInputPin(i)->GetType());
            }

            onyxU32 outputPinCount = node.GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                metaContainer.OutputPins.push_back(node.GetOutputPin(i)->GetType());
            }

            m_RegisteredNodes[typeId] = [=]()
                {
                    NodeT* newNode = new NodeT();
                    return newNode;
                };
        }

        MetaDataContainerT& GetNodeMetaData(StringId32 typeId)
        {
            ONYX_ASSERT(m_RegisteredNodesMetaData.contains(typeId), "Node with that ID is not registered.");
            return m_RegisteredNodesMetaData.at(typeId);
        }

        const HashMap<StringId32, MetaDataContainerT>& GetRegisteredNodesMetaData() { return m_RegisteredNodesMetaData; }
        const HashSet<StringId32>& GetRegisteredNodeIds() const { return m_RegisteredNodeTypeIds; }

    protected:
        HashSet<StringId32> m_RegisteredNodeTypeIds;
        HashMap<StringId32, InplaceFunction<Node*()>> m_RegisteredNodes;
        HashMap<StringId32, MetaDataContainerT> m_RegisteredNodesMetaData;
    };

    template <typename NodeType, typename MetaDataType>
    class TypedNodeFactory : public INodeFactory
    {
    public:
        using NodeTypeT = NodeType;

        template <Details::IsNodeGraphNode T>
        static void Register()
        {
            ms_NodeRegistry.template Register<T>();
        }

        UniquePtr<Node> CreateNode(StringId32 typeHash) const override
        {
            return ms_NodeRegistry.CreateNode(typeHash);
        }

        const NodeEditorMetaData& GetNodeMetaData(StringId32 typeHash) const override
        {
            return ms_NodeRegistry.GetNodeMetaData(typeHash);
        }

        const HashSet<StringId32>& GetRegisteredNodeIds() const override
        {
            return ms_NodeRegistry.GetRegisteredNodeIds();
        }

    protected:
        static NodeRegistry<MetaDataType> ms_NodeRegistry;
    };

    template<typename NodeType, typename MetaDataType>
    NodeRegistry<MetaDataType> TypedNodeFactory<NodeType, MetaDataType>::ms_NodeRegistry;

    class NodeGraphFactory : public TypedNodeFactory<Node, NodeEditorMetaData>
    {
    public:
        //TODO: Node concept to enforce node
        template <typename T>
        static void Register()
        {
            TypedNodeFactory::Register<T>();
        }
    };
}
