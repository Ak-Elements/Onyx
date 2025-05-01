#pragma once

#include <onyx/nodegraph/node.h>
#include <onyx/nodegraph/pin.h>
#include <onyx/filesystem/onyxfile.h>

#include <any>

namespace Onyx::FileSystem
{
    struct JsonValue;
}

namespace Onyx::NodeGraph
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
    }

    enum class GraphContext : onyxU32
    {
        Invalid,
        NodeGraph,
        ShaderGraph,
    };

    struct NodeEditorMetaData
    {
        onyxU32 TypeId;
        String FullyQualifiedName;
        DynamicArray<PinTypeId> InputPins;
        DynamicArray<PinTypeId> OutputPins;
    };

    class NodeGraphTypeRegistry
    {
    public:
        template <typename T> requires HasTypeId<T>
        static void RegisterType()
        {
            constexpr StringId32 serializedTypeId = T::TypeId;
            ONYX_ASSERT(s_RegisteredTypes.contains(serializedTypeId) == false, "Type is already registered in this context");

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
            s_RegisteredTypes[serializedTypeId] = [](Guid64 globalId, onyxU32 localId, const String& localIdString) { return new DynamicPin<T>(globalId, localId, localIdString); };
#else
            s_RegisteredTypes[serializedTypeId] = [](Guid64 globalId, onyxU32 localId) { return new DynamicPin<T>(globalId, localId); };
#endif
        }

        template <typename T, CompileTimeString SerializedTypeId>
        static void RegisterType()
        {
            constexpr StringId32 serializedTypeId(SerializedTypeId);
            ONYX_ASSERT(s_RegisteredTypes.contains(serializedTypeId) == false, "Type is already registered in this context");

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
            s_RegisteredTypes[serializedTypeId] = [](Guid64 globalId, onyxU32 localId, const String& localIdString) { return new DynamicPin<T>(globalId, localId, localIdString); };
#else
            s_RegisteredTypes[serializedTypeId] = [](Guid64 globalId, onyxU32 localId) { return new DynamicPin<T>(globalId, localId); };
#endif
        }

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        static UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, onyxU32 localId, const String& localIdString)
        {
            return UniquePtr<PinBase>(s_RegisteredTypes.at(typeId)(globalId, localId, localIdString));
        }
#else
        static UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, onyxU32 localId)
        {
            return UniquePtr<PinBase>(s_RegisteredTypes.at(typeId)(globalId, localId));
        }
#endif

    private:
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        static HashMap<StringId32, InplaceFunction<PinBase*(Guid64, onyxU32, const String&)>> s_RegisteredTypes;
#else
        static HashMap<StringId32, InplaceFunction<PinBase* (Guid64, onyxU32)>> s_RegisteredTypes;
#endif
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
        void RegisterNode(const StringView& fullyQualifiedName) 
        {
            static HashMap<onyxU32, String> s_RegisteredNodesToName;

            constexpr StringId32 typeId = NodeT::TypeId;
            ONYX_ASSERT(m_RegisteredNodeTypeIds.contains(typeId) == false, "Node is already registered in this context");

            m_RegisteredNodeTypeIds.emplace(typeId);
            m_RegisteredNodesToName[typeId] = NodeT::TypeId.IdString;
            MetaDataContainerT& metaContainer = m_RegisteredNodesMetaData[typeId];
            metaContainer.FullyQualifiedName = fullyQualifiedName;

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
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
                newNode->SetTypeName(TypeName<NodeT>());

                StringView::size_type index = fullyQualifiedName.find_last_of('/');
                newNode->SetName((index == StringView::npos) ? String(fullyQualifiedName) : String(fullyQualifiedName.substr(index + 1)));
#endif

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
        HashMap<StringId32, String> m_RegisteredNodesToName;
    };

    class INodeFactory
    { 
    public:
        virtual ~INodeFactory() = default;
        virtual UniquePtr<Node> CreateNode(StringId32 typeId) const = 0;
        virtual const HashSet<StringId32>& GetRegisteredNodeIds() const = 0;
        virtual const NodeEditorMetaData& GetNodeMetaData(StringId32 typeId) const = 0;
    };

    template <typename NodeType, typename MetaDataType>
    class TypedNodeFactory : public INodeFactory
    {
    public:
        using NodeTypeT = NodeType;

        template <Details::IsNodeGraphNode T>
        static void RegisterNode(const StringView& nodeName)
        {
            ms_NodeRegistry.template RegisterNode<T>(nodeName);
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
        static void RegisterNode(const StringView& nodeName)
        {
            TypedNodeFactory::RegisterNode<T>(nodeName);
        }
    };
}
