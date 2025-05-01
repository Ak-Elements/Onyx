#include <editor/nodegraph/nodegrapheditorcontext.h>

#include <editor/modules/nodeeditor.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphserializer.h>
#include <onyx/thread/threadpool/threadpool.h>

namespace Onyx::Editor
{
    struct DefaultNodeFilter
    {
        bool operator()(NodeGraph::NodeEditorMetaData&) const
        {
            return true;
        }
    };

    void NodeGraphEditorContext::OnDrawNode(const Node& node)
    {
        NodeGraph::Node& nodeGraphNode = GetNodeGraphNode(node.LocalId);
        nodeGraphNode.UIDrawNode();
    }

    
    bool NodeGraphEditorContext::IsNewLinkValid(Guid64 fromPinId, Guid64 toPinId) const
    {
        return GetNodeGraph().IsNewLinkValid(fromPinId, toPinId) == false;
    }

    bool NodeGraphEditorContext::ArePinTypesCompatible(NodeGraph::PinTypeId lhsPinType, NodeGraph::PinTypeId rhsPinType) const
    {
        return lhsPinType == rhsPinType;
    }

    void NodeGraphEditorContext::DrawNodeInPropertyPanel(Guid64 nodeId)
    {
        Node& editorNode = GetNode(nodeId);
        NodeGraph::Node& graphNode = GetNodeGraphNode(editorNode.LocalId);
        HashMap<onyxU64, std::any>& constantPinData = GetNodeGraph().GetConstantPinData();
        // TODO: Change bool to enum - NodeChanged::PinAdded, NodeChanged::PinRemoved NodeChanged::PinRenamed etc...
        bool hasChanged = graphNode.DrawInPropertyGrid(constantPinData);

        if (hasChanged)
        {
            UpdateEditorNodeData(editorNode, graphNode);
        }
    }

    void NodeGraphEditorContext::FilterNodeListContextMenu(InplaceFunction<bool(const NodeGraph::NodeEditorMetaData& nodeMeta)> filterFunctor)
    {
        m_ContextMenuRoot.m_Children.clear();

        const NodeGraph::INodeFactory& factory = GetNodeFactory();
        const auto& nodeTypeIds = factory.GetRegisteredNodeIds();

        for (const StringId32 typeId : nodeTypeIds)
        {
            const NodeGraph::NodeEditorMetaData& nodeMetaData = factory.GetNodeMetaData(typeId);

            if (filterFunctor && filterFunctor(nodeMetaData) == false)
            {
                continue;
            }

            constexpr char delimiter = '/';
            DynamicArray<String> split = Split(nodeMetaData.FullyQualifiedName, delimiter);

            NodeListContextMenuItem* currentParent = &m_ContextMenuRoot;
            for (onyxU32 i = 0; i < split.size(); ++i)
            {
                const String& currentToken = split[i];
                if (i == split.size() - 1)
                {
                    NodeListContextMenuItem& menuItem = currentParent->m_Children[currentToken];
                    menuItem.Label = currentToken;
                    menuItem.TypeId = typeId;
                    break;
                }

                currentParent = &currentParent->m_Children[currentToken];
                currentParent->Label = currentToken;
            }
        }
    }

    void NodeGraphEditorContext::ClearNodeListFilter()
    {
        FilterNodeListContextMenu(nullptr);
    }

    const GraphEditorContext::NodeListContextMenuItem& NodeGraphEditorContext::GetNodeListContextMenuRoot()
    {
        return m_ContextMenuRoot;
    }
    
    void NodeGraphEditorContext::UpdateEditorNodeData(Node& editorNode, const NodeGraph::Node& graphNode)
    {
        DynamicArray<Link>& editorLinks = GetLinks();
        const onyxU32 inputPinCount = graphNode.GetInputPinCount();
        editorNode.Inputs.reserve(inputPinCount);
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const NodeGraph::PinBase* inputPin = graphNode.GetInputPin(i);

            Pin& inputPinEditorMeta = editorNode.Inputs.emplace_back();
            inputPinEditorMeta.Name = graphNode.GetPinName(inputPin->GetLocalId());
            inputPinEditorMeta.Id = inputPin->GetGlobalId();
            inputPinEditorMeta.LocalId = inputPin->GetLocalId();
            inputPinEditorMeta.PinTypeId = inputPin->GetType();
            inputPinEditorMeta.Color = inputPin->GetTypeColor();
            inputPinEditorMeta.Direction = PinDirection::Input;

            if (inputPin->IsConnected())
            {
                editorLinks.emplace_back(Guid64Generator::GetGuid(),
                    inputPin->GetGlobalId(),
                    inputPin->GetLinkedPinGlobalId(),
                    graphNode.GetId(),
                    GetNodeGraph().GetNodeForPinId(inputPin->GetLinkedPinGlobalId()).GetId(),
                    inputPin->GetTypeColor());
            }
        }

        const onyxU32 outputPinCount = graphNode.GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const NodeGraph::PinBase* outputPin = graphNode.GetOutputPin(i);

            Pin& outputPinEditorMeta = editorNode.Outputs.emplace_back();
            outputPinEditorMeta.Name = graphNode.GetPinName(outputPin->GetLocalId());
            outputPinEditorMeta.Id = outputPin->GetGlobalId();
            outputPinEditorMeta.LocalId = outputPin->GetLocalId();
            outputPinEditorMeta.PinTypeId = outputPin->GetType();
            outputPinEditorMeta.Color = outputPin->GetTypeColor();
            outputPinEditorMeta.Direction = PinDirection::Output;

            if (outputPin->IsConnected())
            {
                editorLinks.emplace_back(Guid64Generator::GetGuid(),
                    outputPin->GetGlobalId(),
                    outputPin->GetLinkedPinGlobalId(),
                    graphNode.GetId(),
                    GetNodeGraph().GetNodeForPinId(outputPin->GetLinkedPinGlobalId()).GetId(),
                    outputPin->GetTypeColor());
            }
        }
    }

    void NodeGraphEditorContext::OnNodeDelete(Node& nodeToDelete)
    {
        GetNodeGraph().Remove(nodeToDelete.LocalId);
    }

    void NodeGraphEditorContext::OnLinkCreate(const Link& newLink)
    {
        NodeGraph::NodeGraph& graph = GetNodeGraph();
        NodeGraph::Node& node = graph.GetNodeForPinId(newLink.FromPinId);

        const onyxU32 inputPinCount = node.GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            NodeGraph::PinBase* inputPin = node.GetInputPin(i);
            if (inputPin->GetGlobalId() != newLink.FromPinId)
                continue;

            // disconnect the connection
            if (inputPin->IsConnected())
            {
                DeleteLink(newLink.FromPinId, inputPin->GetLinkedPinGlobalId());
            }

            graph.AddEdge(newLink.ToPinId, newLink.FromPinId);
            inputPin->ConnectPin(newLink.ToPinId);
            return;
        }

        const onyxU32 outputPinCount = node.GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            NodeGraph::PinBase* outputPin = node.GetOutputPin(i);
            if (outputPin->GetGlobalId() != newLink.FromPinId)
                continue;

            // disconnect the connection
            if (outputPin->IsConnected())
            {
                DeleteLink(newLink.FromPinId, outputPin->GetLinkedPinGlobalId());
            }

            graph.AddEdge(newLink.FromPinId, newLink.ToPinId);
            outputPin->ConnectPin(newLink.ToPinId);
            return;
        }

        ONYX_ASSERT(false, "Failed creating link");
    }

    
    void NodeGraphEditorContext::OnLinkDelete(const Link& link)
    {
        NodeGraph::Node& node = GetNodeGraph().GetNodeForPinId(link.FromPinId);

        const onyxU32 inputPinCount = node.GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            NodeGraph::PinBase* inputPin = node.GetInputPin(i);
            if (inputPin->GetGlobalId() != link.FromPinId)
                continue;

            inputPin->ClearLink();
            return;
        }

        const onyxU32 outputPinCount = node.GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            NodeGraph::PinBase* outputPin = node.GetOutputPin(i);
            if (outputPin->GetGlobalId() != link.FromPinId)
                continue;

            outputPin->ClearLink();
            return;
        }

        ONYX_ASSERT(false, "Failed clearing link from graph");
    }

    
    bool NodeGraphEditorContext::OnNodeCreate(Node& newEditorNode, StringId32 typeId)
    {
        UniquePtr<NodeGraph::Node> newNode = GetNodeFactory().CreateNode(typeId);
        newNode->SetId(newEditorNode.Id);

        newEditorNode.Name = newNode->GetName();
        UpdateEditorNodeData(newEditorNode, *newNode);

        newEditorNode.LocalId = GetNodeGraph().Emplace(std::move(newNode));
        return true;
    }
}
