#include <onyx/graphics/serialize/rendergraphserializer.h>

#include <onyx/graphics/textureasset.h>
#include <onyx/filesystem/imagefile.h>
#include <onyx/graphics/rendergraph/rendergraphnodefactory.h>

namespace Onyx
{
    class UIRenderGraphNode;
}

namespace Onyx::Graphics
{
    RenderGraphSerializer::RenderGraphSerializer(Assets::AssetSystem& assetSystem, GraphicsApi& graphicsApi)
        : AssetSerializer(assetSystem)
        , m_GraphicsApi(&graphicsApi)
    {
    }

    bool RenderGraphSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool RenderGraphSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
#if ONYX_IS_EDITOR
        const RenderGraph& renderGraph = asset.As<RenderGraph>();
        const NodeGraph::NodeGraph& nodeGraph = renderGraph.GetNodeGraph();

        FileSystem::JsonValue jsonRoot;

        // serialize
        using NodeId = DirectedAcyclicGraph<UniquePtr<IRenderGraphNode>>::NodeId;
        DynamicArray<NodeId> sortedNodes = nodeGraph.GetTopologicalOrder();

        for (NodeId nodeId : sortedNodes)
        {
            const IRenderGraphNode& node = nodeGraph.GetNode<IRenderGraphNode>(nodeId);
            
            FileSystem::JsonValue nodeJsonObject;
            node.Serialize(nodeJsonObject);

            jsonRoot.Add(nodeJsonObject);
        }

        const String& jsonString = jsonRoot.Json.dump(4);
        using namespace FileSystem;
        OnyxFile graphOutFile(filePath);
        FileStream stream = graphOutFile.OpenStream(OpenMode::Write | OpenMode::Text);
        stream.WriteRaw(jsonString.data(), jsonString.size());

        return true;
#else
        ONYX_UNUSED(asset);
        ONYX_UNUSED(filePath);
        return false;
#endif
    }

    bool RenderGraphSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool RenderGraphSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }

    bool RenderGraphSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        if (m_GraphicsApi == nullptr)
            return false;

        RenderGraphNodeFactory factory;

        RenderGraph& renderGraph = asset.As<RenderGraph>();
        renderGraph.Shutdown(*m_GraphicsApi);

        NodeGraph::NodeGraph& graph = renderGraph.GetNodeGraph();
        
        FileSystem::OnyxFile graphFile(filePath);
        const FileSystem::JsonValue& jsonRoot = graphFile.LoadJson();

        FileSystem::JsonValue graphStructureJson;
        FileSystem::JsonValue constantPinDataJson;

        HashMap<Guid64, Guid64> edges;

        for (const auto& nodeJson : jsonRoot.Json)
        {
            FileSystem::JsonValue nodeJsonObj{ nodeJson };

            onyxU32 typeId = 0;
            nodeJsonObj.Get("typeId", typeId);

            UniquePtr<NodeGraph::Node> node = factory.CreateNode(typeId);
            node->Deserialize(nodeJsonObj);

            const onyxU32 inputPinCount = node->GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                NodeGraph::PinBase* pin = node->GetInputPin(i);
                if (pin->IsConnected())
                {
                    edges[pin->GetGlobalId()] = pin->GetLinkedPinGlobalId();
                    continue;
                }

                FileSystem::JsonValue dataJson;
                Guid64 globalId = pin->GetGlobalId();
                const StringView& globalIdString = Format::Format("0x{:x}", globalId.Get());
                if (constantPinDataJson.Get(globalIdString, dataJson))
                {
                    std::any value = pin->CreateDefault();
                    pin->Deserialize(dataJson, value);
                }
            }

            const onyxU32 outputPinCount = node->GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                NodeGraph::PinBase* pin = node->GetOutputPin(i);
                if (pin->IsConnected() == false)
                {
                    continue;
                }
            }

            UniquePtr<IRenderGraphNode> newRenderNode(static_cast<IRenderGraphNode*>(node.release()));
            graph.Emplace(std::move(newRenderNode));
        }

        for (auto&& [fromPinId, toPinId] : edges)
        {
            bool isEdgeValid = graph.AddEdge(toPinId, fromPinId);

            if (isEdgeValid == false)
            {
                ONYX_ASSERT(false, "Trying to add invalid edge, there is an error in the saved render graph {}", filePath);
                return false;
            }
        }

        return true;
    }

    bool RenderGraphSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }
}
