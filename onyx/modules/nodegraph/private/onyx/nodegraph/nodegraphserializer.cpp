#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>
#include <onyx/nodegraph/pins/pin.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx::NodeGraph
{
    bool Serialize(Serializer& serializer, const NodeGraph& nodeGraph)
    {
#if ONYX_IS_EDITOR
        // serialize
        DynamicArray<const Node*> nodes = nodeGraph.GetNodesSorted();
        const HashMap<Guid64, std::any>& constantPinData = nodeGraph.GetConstantPinData();

        serializer.WriteForEach<"graph">(nodes, [](Serializer& scopedSerializer, const Node* node)
        {
            return node->Serialize(scopedSerializer);
        });

        serializer.WriteForEach<"data">(constantPinData, [&](Serializer& scopedSerializer, const Guid64& globalPinId, const std::any& value)
        {
            const PinBase& pin = nodeGraph.GetPinById(globalPinId);
            const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::GetTypeMeta(pin.GetType());
            return typeMeta.Serialize(scopedSerializer, value);
        });

        return true;
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(json);
        return false;
#endif
    }

    bool Deserialize(const Deserializer& deserializer, NodeGraph& outNodeGraph, const INodeFactory& nodeFactory)
    {
        HashMap<Guid64, Guid64> edges;
        HashMap<Guid64, std::any>& constantPinData = outNodeGraph.GetConstantPinData();

        bool success = deserializer.ReadForEach<"graph">([&](const Deserializer& scopedDeserializer)
        {
            StringId32 typeId;
            if (scopedDeserializer.Read<"typeId">(typeId) == false)
            {
                return false;
            }

            UniquePtr<Node> node = nodeFactory.CreateNode(typeId);
            if (node->Deserialize(scopedDeserializer) == false)
            {
                return false;
            }

            const onyxU32 inputPinCount = node->GetInputPinCount();
            for (onyxU32 inputPinIndex = 0; inputPinIndex < inputPinCount; ++inputPinIndex)
            {
                PinBase* pin = node->GetInputPin(inputPinIndex);
                if (pin->IsConnected())
                {
                    edges[pin->GetGlobalId()] = pin->GetLinkedPinGlobalId();
                }
            }

            const onyxU32 outputPinCount = node->GetOutputPinCount();
            for (onyxU32 outputPinIndex = 0; outputPinIndex < outputPinCount; ++outputPinIndex)
            {
                PinBase* pin = node->GetOutputPin(outputPinIndex);
                if (pin->IsConnected())
                {
                    edges[pin->GetGlobalId()] = pin->GetLinkedPinGlobalId();
                }
            }

            outNodeGraph.Emplace(std::move(node));
            return true;
        });

        if (success == false)
        {
            return false;
        }
        
        deserializer.ReadForEach<"data">(constantPinData, [&](const Deserializer& scopedDeserializer, const Guid64& globalPinId, std::any& outValue)
        {
            const PinBase& pin = outNodeGraph.GetPinById(globalPinId);
            const INodeGraphTypeMeta& typeMeta = NodeGraphTypeRegistry::GetTypeMeta(pin.GetType());
            return typeMeta.Deserialize(scopedDeserializer, outValue);
        });

        for (auto&& [fromPinId, toPinId] : edges)
        {
            bool isEdgeValid = outNodeGraph.AddEdge(toPinId, fromPinId);

            if (isEdgeValid == false)
            {
                ONYX_ASSERT(false, "Trying to add invalid edge, there is an error in the saved graph");
                return false;
            }
        }

        return success;
    }
}
