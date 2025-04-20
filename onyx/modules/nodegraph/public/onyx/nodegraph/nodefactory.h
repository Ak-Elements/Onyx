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
    namespace Details
    {
        template <typename T>
        concept IsNodeGraphNode = requires(T& node, ExecutionContext& context, Guid64 globalPinId)
        {
            //{ node.Prepare() };
            //{ node.Update(context) };
            //{ node.Finish() };
            { node.HasPin(globalPinId) };
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
        onyxU32 TypeId;
        String FullyQualifiedName;
        DynamicArray<PinTypeId> InputPins;
        DynamicArray<PinTypeId> OutputPins;
    };

    template <typename MetaDataContainerT = NodeEditorMetaData>
    requires std::is_base_of_v<NodeEditorMetaData, MetaDataContainerT>
    class NodeRegistry
    {
        // serialize / deserialize functors
    public:
        UniquePtr<Node> CreateNode(onyxU32 typeHash)
        {
            ONYX_ASSERT(m_RegisteredNodes.contains(typeHash), "Node is not registered in this context");
            return UniquePtr<Node>(m_RegisteredNodes.at(typeHash)()); // call functor to create new node
        }

        static bool SerializeType(PinTypeId typeId, const std::any& instance, FileSystem::JsonValue& outJsonValue)
        {
            const GraphTypeInfo& typeInfo = m_RegisteredTypes.at(static_cast<onyxU32>(typeId));
            return typeInfo.SerializeToJsonFunctor(instance, outJsonValue);
        }

        template <Details::IsNodeGraphNode NodeT>
        void RegisterNode(const StringView& fullyQualifiedName) 
        {
            constexpr onyxU32 typeHash = TypeHash<NodeT>();
            ONYX_ASSERT(m_RegisteredNodeTypeIds.contains(typeHash) == false, "Node is already registered in this context");

            m_RegisteredNodeTypeIds.emplace(typeHash);

            MetaDataContainerT& metaContainer = m_RegisteredNodesMetaData[typeHash];
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

            m_RegisteredNodes[typeHash] = [=]()
            {
                NodeT* newNode = new NodeT();
                newNode->SetTypeId(typeHash);
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
                newNode->SetTypeName(TypeName<NodeT>());

                StringView::size_type index = fullyQualifiedName.find_last_of('/');
                newNode->SetName((index == StringView::npos) ? String(fullyQualifiedName) : String(fullyQualifiedName.substr(index + 1)));
#endif

                return newNode;
            };
        }

        static bool DeserializeType(PinTypeId typeId, std::any& instance, const FileSystem::JsonValue& inJsonValue)
        {
            const GraphTypeInfo& typeInfo = m_RegisteredTypes.at(static_cast<onyxU32>(typeId));
            return typeInfo.DeserializeFromJsonFunctor(instance, inJsonValue);
        }

        static std::any CreateDefaultForType(PinTypeId typeId)
        {
            const GraphTypeInfo& typeInfo = m_RegisteredTypes.at(static_cast<onyxU32>(typeId));
            return typeInfo.CreateDefaultFunctor();
        }

        MetaDataContainerT& GetNodeMetaData(onyxU32 typeId)
        {
            ONYX_ASSERT(m_RegisteredNodesMetaData.contains(typeId), "Node with that ID is not registered.");
            return m_RegisteredNodesMetaData.at(typeId);
        }

        const HashMap<onyxU32, MetaDataContainerT>& GetRegisteredNodesMetaData() { return m_RegisteredNodesMetaData; }
        const HashSet<onyxU32>& GetRegisteredNodeIds() const { return m_RegisteredNodeTypeIds; }

    protected:
        struct GraphTypeInfo
        {
            InplaceFunction<std::any()> CreateDefaultFunctor;
            InplaceFunction<bool(const std::any&, FileSystem::JsonValue&)> SerializeToJsonFunctor;
            InplaceFunction<bool(std::any&, const FileSystem::JsonValue&)> DeserializeFromJsonFunctor;
        };

        HashSet<onyxU32> m_RegisteredNodeTypeIds;
        HashMap<onyxU32, InplaceFunction<Node*()>> m_RegisteredNodes;
        HashMap<onyxU32, MetaDataContainerT> m_RegisteredNodesMetaData;

        static HashMap<onyxU32, GraphTypeInfo> m_RegisteredTypes;
    };

    class INodeFactory
    { 
    public:
        virtual ~INodeFactory() = default;
        virtual UniquePtr<Node> CreateNode(onyxU32 typeHash) const = 0;
        virtual const HashSet<onyxU32>& GetRegisteredNodeIds() const = 0;
        virtual const NodeEditorMetaData& GetNodeMetaData(onyxU32 typeHash) const = 0;
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

        UniquePtr<Node> CreateNode(onyxU32 typeHash) const override
        {
            return ms_NodeRegistry.CreateNode(typeHash);
        }

        const NodeEditorMetaData& GetNodeMetaData(onyxU32 typeHash) const override
        {
            return ms_NodeRegistry.GetNodeMetaData(typeHash);
        }

        const HashSet<onyxU32>& GetRegisteredNodeIds() const override
        {
            return ms_NodeRegistry.GetRegisteredNodeIds();
        }

    protected:
        static NodeRegistry<MetaDataType> ms_NodeRegistry;
    };

    class NodeFactory : public TypedNodeFactory<Node, NodeEditorMetaData>
    {
    };

    template <>
    NodeRegistry<NodeEditorMetaData> TypedNodeFactory<Node, NodeEditorMetaData>::ms_NodeRegistry;
}
