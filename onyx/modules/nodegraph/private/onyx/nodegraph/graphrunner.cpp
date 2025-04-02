#include <onyx/nodegraph/graphrunner.h>

#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/node.h>

namespace Onyx::NodeGraph
{
    void GraphRunner::Prepare()
    {
        const HashMap<onyxU64, std::any>& constantPinData = m_Graph->GetConstantPinData();

        const DynamicArray<onyxS8>& executionOrder = m_Graph->GetTopologicalOrder();

        // collect pin meta information first
        HashSet<Guid64> connectedPins;
        for (onyxS8 localNodeId : executionOrder)
        {
            const Node& node = m_Graph->GetNode(localNodeId);

            const onyxU32 inputPinCount = node.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* inputPin = node.GetInputPin(i);
                if (inputPin->IsConnected())
                {
                    connectedPins.emplace(inputPin->GetLinkedPinGlobalId());
                }
            }

            const onyxU32 outputPinCount = node.GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const PinBase* outputPin = node.GetOutputPin(i);
                if (outputPin->IsConnected())
                {
                    connectedPins.emplace(outputPin->GetLinkedPinGlobalId());
                }
            }
        }

        for (onyxS8 localNodeId : executionOrder)
        {
            const Node& node = m_Graph->GetNode(localNodeId);
            node.Prepare(m_PrepareContext);

            // setup execution context
            ExecutionContext::NodeContext context;

            const onyxU32 inputPinCount = node.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* inputPin = node.GetInputPin(i);
                Guid64 globalPinId = inputPin->GetGlobalId();

                if (constantPinData.contains(globalPinId))
                {
                    context.PinData[inputPin->GetLocalId()] = constantPinData.at(globalPinId);
                }
                else
                {
                    context.PinData[inputPin->GetLocalId()] = inputPin->CreateDefault();
                }

                context.PinMetaData[inputPin->GetLocalId()].IsConnected = inputPin->IsConnected() || connectedPins.contains(inputPin->GetGlobalId());
            }

            const onyxU32 outputPinCount = node.GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const PinBase* outputPin = node.GetOutputPin(i);
                context.PinData[outputPin->GetLocalId()] = outputPin->CreateDefault();

                context.PinMetaData[outputPin->GetLocalId()].IsConnected = outputPin->IsConnected() || connectedPins.contains(outputPin->GetGlobalId());
            }

            m_ExecutionContext.AddNodeContext(node.GetId(), context);
        }
    }

    void GraphRunner::Update(onyxU64 deltaTime)
    {
        ONYX_UNUSED(deltaTime);

        const DynamicArray<onyxS8>& executionOrder = m_Graph->GetTopologicalOrder();
        for (onyxS8 localNodeId : executionOrder)
        {
            const Node& node = m_Graph->GetNode(localNodeId);

            ExecutionContext::NodeContext& currentContext = m_ExecutionContext.SetCurrentNode(node.GetId());

            const onyxU32 inputPinCount = node.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* inputPin = node.GetInputPin(i);
                if (inputPin->IsConnected())
                {
                    Guid64 linkedPinId = inputPin->GetLinkedPinGlobalId();
                    const Node& connectedNode = m_Graph->GetNodeForPinId(linkedPinId);
                    const ExecutionContext::NodeContext& dependantNodeContext = m_ExecutionContext.GetNodeContext(connectedNode.GetId());

                    const onyxU32 outputPinCount = connectedNode.GetOutputPinCount();
                    for (onyxU32 outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex)
                    {
                        const PinBase* outputPin = connectedNode.GetOutputPin(outputPinIndex);
                        if (outputPin->GetGlobalId() == linkedPinId)
                        {
                            currentContext.PinData[inputPin->GetLocalId()] = dependantNodeContext.PinData.at(outputPin->GetLocalId());
                            break;
                        }
                    }
                }
            }

            node.Update(m_ExecutionContext);
        }
    }

    void GraphRunner::Shutdown()
    {
    }
}
