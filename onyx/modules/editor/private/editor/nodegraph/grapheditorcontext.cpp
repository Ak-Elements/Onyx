#include <editor/nodegraph/grapheditorcontext.h>
#include <onyx/assets/asset.h>

namespace Onyx::Editor
{
    void GraphEditorContext::Clear()
    {
        m_Nodes.clear();
        m_Links.clear();
    }

    GraphEditorContext::Node& GraphEditorContext::CreateNewNode(onyxU32 nodeTypeId)
    {
        Guid64 newNodeId = Guid64Generator::GetGuid();
        Node tmpNode(newNodeId);
        bool hasSucceeded = OnNodeCreate(tmpNode, nodeTypeId);

        if (hasSucceeded == false)
        {
            ONYX_ASSERT(false, "Failed creating node");
            return m_Nodes.back();
        }

        Node& newNode = m_Nodes.emplace_back(std::move(tmpNode));
        if (OnNodeCreated)
        {
            OnNodeCreated(newNode);
        }

        return newNode;
    }

    void GraphEditorContext::DeleteNode(Guid64 nodeId)
    {
        auto it = std::ranges::find_if(m_Nodes, [&](const Node& node)
        {
            return node.Id == nodeId;
        });

        if (it == m_Nodes.end())
            return;

        for (auto linksIt = m_Links.begin(); linksIt != m_Links.end();)
        {
            const Link& link = *linksIt;
            if (link.FromNodeId == nodeId)
            {
                OnLinkDelete(link);
                linksIt = m_Links.erase(linksIt);
                continue;
            }

            if (link.ToNodeId == nodeId)
            {
                OnLinkDelete(link);
                linksIt = m_Links.erase(linksIt);
                continue;
            }

            ++linksIt;
        }

        Node& editorNode = *it;

        
        OnNodeDelete(editorNode);

        if (OnNodeDeleted)
        {
            OnNodeDeleted(editorNode);
        }

        m_Nodes.erase(it);
    }


    GraphEditorContext::Node& GraphEditorContext::GetNode(Guid64 nodeId)
    {
        auto it = std::ranges::find_if(m_Nodes, [&](Node& node)
            {
                return node.Id == nodeId;
            });

        return *it;
    }

    const GraphEditorContext::Node& GraphEditorContext::GetNode(Guid64 nodeId) const
    {
        auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&](const Node& node)
            {
                return node.Id == nodeId;
            });

        return *it;
    }

    GraphEditorContext::Node& GraphEditorContext::GetNodeForPin(Guid64 pinId)
    {
        for (Node& node : m_Nodes)
        {
            for (Pin& inputPin : node.Inputs)
            {
                if (inputPin.Id == pinId)
                    return node;
            }

            for (Pin& outputPin : node.Outputs)
            {
                if (outputPin.Id == pinId)
                    return node;
            }
        }

        ONYX_ASSERT(false, "Failed finding pin with ID {}", pinId.Get());
        return m_Nodes[0];
    }

    const GraphEditorContext::Node& GraphEditorContext::GetNodeForPin(Guid64 pinId) const
    {
        for (const Node& node : m_Nodes)
        {
            for (const Pin& inputPin : node.Inputs)
            {
                if (inputPin.Id == pinId)
                    return node;
            }

            for (const Pin& outputPin : node.Outputs)
            {
                if (outputPin.Id == pinId)
                    return node;
            }
        }

        ONYX_ASSERT(false, "Failed finding pin with ID {}", pinId.Get());
        return m_Nodes[0];
    }

    void GraphEditorContext::SetNodeName(Guid64 nodeId, const String& name)
    {
        Node& node = GetNode(nodeId);
        node.Name = name;
    }

    void GraphEditorContext::SetNodePosition(Guid64 nodeId, const Vector2f& position)
    {
        Node& node = GetNode(nodeId);

        if (node.Position != position)
        {
            node.Position = position;
            node.HasUpdatedPosition = true;
        }
    }

    GraphEditorContext::Link& GraphEditorContext::CreateNewLink(Guid64 fromPinId, Guid64 toPinId)
    {
        Link newLink(Guid64Generator::GetGuid(),
            fromPinId,
            toPinId,
            GetNodeForPin(fromPinId).Id,
            GetNodeForPin(toPinId).Id,
            GetPin(fromPinId).Color);

        OnLinkCreate(newLink);

        if (OnLinkCreated)
        {
            OnLinkCreated(newLink);
        }

        return m_Links.emplace_back(newLink);
    }

    void GraphEditorContext::DeleteLink(Guid64 linkId)
    {
        auto it = std::find_if(m_Links.begin(), m_Links.end(), [&](const Link& link)
            {
                return link.Id == linkId;
            });

        if (it == m_Links.end())
            return;

        const Link& link = *it;

        OnLinkDelete(link);

        if (OnLinkDeleted)
        {
            OnLinkDeleted(link);
        }

        m_Links.erase(it);
    }

    void GraphEditorContext::DeleteLink(Guid64 fromPinId, Guid64 toPinId)
    {
        auto it = std::find_if(m_Links.begin(), m_Links.end(), [&](const Link& link)
        {
            return (link.FromPinId == fromPinId) && (link.ToPinId == toPinId);
        });

        if(it == m_Links.end())
            return;

        const Link& link = *it;

        OnLinkDelete(link);

        if (OnLinkDeleted)
        {
            OnLinkDeleted(link);
        }

        m_Links.erase(it);
    }

    GraphEditorContext::Pin& GraphEditorContext::GetPin(Guid64 pinId)
    {
        for (Node& node : m_Nodes)
        {
            for (Pin& inputPin : node.Inputs)
            {
                if (inputPin.Id == pinId)
                    return inputPin;
            }

            for (Pin& outputPin : node.Outputs)
            {
                if (outputPin.Id == pinId)
                    return outputPin;
            }
        }

        ONYX_ASSERT(false, "Failed getting pin for id");
        return m_Nodes[0].Inputs[0];
    }

    const GraphEditorContext::Pin& GraphEditorContext::GetPin(Guid64 pinId) const
    {
        for (const Node& node : m_Nodes)
        {
            for (const Pin& inputPin : node.Inputs)
            {
                if (inputPin.Id == pinId)
                    return inputPin;
            }

            for (const Pin& outputPin : node.Outputs)
            {
                if (outputPin.Id == pinId)
                    return outputPin;
            }
        }

        ONYX_ASSERT(false, "Failed getting pin for id");
        return m_Nodes[0].Inputs[0];
    }

    bool GraphEditorContext::IsPinLinked(Guid64 pindId) const
    {
        return std::ranges::any_of(m_Links, [&](const Link& link)
        {
            return (link.FromPinId == pindId) || (link.ToPinId == pindId);
        });
    }

    void GraphEditorContext::DrawNode(const Node& node)
    {
        OnDrawNode(node);
    }

    void GraphEditorContext::Load(Assets::AssetSystem& assetSystem, const FileSystem::Filepath& path)
    {
        m_IsLoading = true;
        OnLoad(assetSystem, path);
    }

    void GraphEditorContext::Save(Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& assetMeta)
    {
        OnSave(assetSystem, assetMeta);

        if (SaveEditorMetaDataFunctor)
        {
            SaveEditorMetaDataFunctor(assetMeta.Path);
        }
    }

    void GraphEditorContext::FinishLoading(const Assets::AssetMetaData& assetMeta)
    {
        if (LoadEditorMetaDataFunctor)
        {
            LoadEditorMetaDataFunctor(assetMeta.Path);
        }

        m_IsLoading = false;
    }

    void GraphEditorContext::OnNodeChanged(const Node& /*newNode*/)
    {
    }
}
