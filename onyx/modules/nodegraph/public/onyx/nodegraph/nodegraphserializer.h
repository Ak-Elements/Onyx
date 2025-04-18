#pragma once
#include <onyx/nodegraph/graph.h>
#include <onyx/nodegraph/nodefactory.h>
#include <onyx/filesystem/path.h>

namespace Onyx::FileSystem
{
    class FileStream;
}

namespace Onyx::NodeGraph
{
    namespace Serializer
    {
        //bool Serialize(const NodeGraph& graph, FileSystem::FileStream& outStream) const;
        bool SerializeJson(const NodeGraph& graph, FileSystem::JsonValue& json);
        //bool SerializeYaml(const NodeGraph& graph, FileSystem::FileStream& outStream) const;
        //bool Deserialize(NodeGraph& graph, const FileSystem::FileStream& inStream) const;

        bool DeserializeJson(NodeGraph& graph, const INodeFactory& nodeFactory, const FileSystem::JsonValue& json);
        //bool DeserializeYaml(NodeGraph& graph, const FileSystem::FileStream& inStream) const;

    };
}
