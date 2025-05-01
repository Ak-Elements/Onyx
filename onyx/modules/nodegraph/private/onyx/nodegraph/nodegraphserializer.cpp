#include <onyx/nodegraph/nodegraphserializer.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::NodeGraph::Serializer
{
    bool SerializeJson(const NodeGraph& graph, FileSystem::JsonValue& json)
    {
#if ONYX_IS_EDITOR
        // serialize
        DynamicArray<onyxS8> nodeIdsOrdered = graph.GetTopologicalOrder();
        const HashMap<onyxU64, std::any>& constantPinData = graph.GetConstantPinData();

        FileSystem::JsonValue graphStructureJson;
        FileSystem::JsonValue constantPinDataJson;

        for (onyxS8 localNodeId : nodeIdsOrdered)
        {
            const Node& node = graph.GetNode(localNodeId);

            FileSystem::JsonValue nodeJsonObject;
            node.Serialize(nodeJsonObject);
           
            const onyxU32 inputPinCount = node.GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* pin = node.GetInputPin(i);
                if (pin->IsConnected())
                    continue;

                if (constantPinData.contains(pin->GetGlobalId()))
                {
                    FileSystem::JsonValue pinDataJson;
                    pin->Serialize(pinDataJson, constantPinData.at(pin->GetGlobalId()));
                    constantPinDataJson.Set(Format::Format("{:x}", pin->GetGlobalId().Get()), pinDataJson);
                }
            }

            graphStructureJson.Add(nodeJsonObject);
        }

        json.Set("graph", graphStructureJson);
        json.Set("data", constantPinDataJson);

        return true;
#else
        ONYX_UNUSED(graph);
        ONYX_UNUSED(json);
        return false;
#endif
    }

    /*static*/ bool DeserializeJson(NodeGraph& graph, const INodeFactory& nodeFactory, const FileSystem::JsonValue& json)
    {
        FileSystem::JsonValue graphStructureJson;
        FileSystem::JsonValue constantPinDataJson;
        json.Get("graph", graphStructureJson);
        json.Get("data", constantPinDataJson);

        HashMap<Guid64, Guid64> edges;
        HashMap<onyxU64, std::any>& constantPinData = graph.GetConstantPinData();

        for (const auto& nodeJson : graphStructureJson.Json)
        {
            FileSystem::JsonValue nodeJsonObj{ nodeJson };

            StringId32 typeId = 0;
            nodeJsonObj.Get("typeId", typeId);

            UniquePtr<Node> node = nodeFactory.CreateNode(typeId);
            node->Deserialize(nodeJsonObj);

            const onyxU32 inputPinCount = node->GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                PinBase* pin = node->GetInputPin(i);
                if (pin->IsConnected())
                {
                    edges[pin->GetGlobalId()] = pin->GetLinkedPinGlobalId();
                    continue;
                }

                FileSystem::JsonValue dataJson; 
                Guid64 globalId = pin->GetGlobalId();
                const StringView& globalIdString = Format::Format("{:x}", globalId.Get());
                if (constantPinDataJson.Get(globalIdString, dataJson))
                {
                    pin->Deserialize(dataJson, constantPinData[globalId]);
                }
            }


            const onyxU32 outputPinCount = node->GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                PinBase* pin = node->GetOutputPin(i);
                if (pin->IsConnected())
                {
                    edges[pin->GetGlobalId()] = pin->GetLinkedPinGlobalId();
                    //continue;
                }
            }

            graph.Emplace(std::move(node));
        }

        for (auto&& [fromPinId, toPinId] : edges)
        {
            bool isEdgeValid = graph.AddEdge(toPinId, fromPinId);

            if (isEdgeValid == false)
            {
                ONYX_ASSERT(false, "Trying to add invalid edge, there is an error in the saved graph");
                return false;
            }
        }

        return true;
    }
}
