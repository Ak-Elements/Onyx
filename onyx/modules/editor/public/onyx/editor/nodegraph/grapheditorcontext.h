#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/nodegraphfactory.h>

namespace Onyx::Ui
{
    struct TreeItem;
}

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::Assets
{
    class AssetSystem;
    struct AssetMetaData;
}

namespace Onyx::Editor
{
    class GraphEditorContext
    {
    public:
        GraphEditorContext() = default;
        virtual ~GraphEditorContext() = default;

        enum class PinDirection
        {
            Invalid,
            Input,
            Output
        };

        struct Pin
        {
            Guid64 Id;
            Guid64 LinkId;
            onyxU32 LocalId;
            String Name;

            PinDirection Direction;
            NodeGraph::PinTypeId PinTypeId;
            onyxU32 Color;
        };

        struct Node
        {
            Node(Guid64 id)
                : Id(id)
            {
            }

            Node(Guid64 id, const String& name)
                : Id(id)
                , Name(name)
            {
            }

            Guid64 Id;
            onyxS8 LocalId = INVALID_INDEX_8;

            String Name;
            Vector2f32 Position;

            DynamicArray<Pin> Inputs;
            DynamicArray<Pin> Outputs;

            bool HasUpdatedPosition = false;
            bool ShowNodeName = true;
        };

        struct Link
        {
            Guid64 Id;

            Guid64 FromPinId;
            Guid64 ToPinId;

            // stored to simplify deleting of nodes
            Guid64 FromNodeId;
            Guid64 ToNodeId;

            onyxU32 Color = 0xFFFFFFFF;

            bool IsRerouted = false;
        };

        void SetLocalizationModule(const Localization::LocalizationModule& localizationModule);
        const Localization::LocalizationModule& GetLocalizationModule() const;

        DynamicArray<Node>& GetNodes() { return m_Nodes; }
        const DynamicArray<Node>& GetNodes() const { return m_Nodes; }
        DynamicArray<Link>& GetLinks() { return m_Links; }
        const DynamicArray<Link>& GetLinks() const { return m_Links; }

        void Clear();

        virtual StringView GetLocalizedAssetTypeName() const = 0;
        virtual DynamicArray<StringView> GetExtensions() const = 0;
        virtual onyxU32 GetCanvasBackgroundColor() const = 0;

        Node& CreateNewNode(StringId32 nodeTypeId);
        void DeleteNode(Guid64 nodeId);

        Node& GetNode(Guid64 nodeId);
        const Node& GetNode(Guid64 nodeId) const;
        Node& GetNodeForPin(Guid64 pinId);
        const Node& GetNodeForPin(Guid64 pinId) const;

        void SetNodeName(Guid64 nodeId, const String& name);
        void SetNodePosition(Guid64 nodeId, const Vector2f32& position);

        virtual bool IsNewLinkValid(Guid64 fromPinId, Guid64 toPinId) const = 0;
        Link& CreateNewLink(Guid64 fromPinId, Guid64 toPinId);
        void DeleteLink(Guid64 linkId);
        void DeleteLink(Guid64 fromPinId, Guid64 toPinId);

        Pin& GetPin(Guid64 pinId);
        const Pin& GetPin(Guid64 pinId) const;
        bool IsPinLinked(Guid64 pindId) const;
        virtual bool ArePinTypesCompatible(NodeGraph::PinTypeId lhsPinType, NodeGraph::PinTypeId rhsPinType) const = 0;

        void DrawNode(const Node& node);
        void DrawNodeBackground(const Node& node);

        virtual void DrawNodeInPropertyPanel(Guid64 nodeId) = 0;

        virtual void FilterNodeListContextMenu(InplaceFunction<bool(StringView, const NodeGraph::NodeEditorMetaData&)> filterFunctor) = 0;
        virtual void ClearNodeListFilter() = 0;
        virtual const Ui::TreeItem& GetNodeListContextMenuRoot() = 0;

        virtual bool Compile() = 0;

        void Load(Assets::AssetSystem& assetSystem, const FileSystem::Filepath& path);
        void Save(Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& assetMeta);
        bool IsLoading() const { return m_IsLoading; };

        Callback<void()> OnLoaded;
        Callback<void()> OnSaved;

        Callback<void(Node&)> OnNodeCreated;
        Callback<void(Node&)> OnNodeDeleted;
        Callback<void(const Link&)> OnLinkCreated;
        Callback<void(const Link&)> OnLinkDeleted;

        Callback<void(const FileSystem::Filepath&)> LoadEditorMetaDataFunctor;
        Callback<void(const FileSystem::Filepath&)> SaveEditorMetaDataFunctor;

    protected:
        virtual const NodeGraph::INodeFactory& GetNodeFactory() const = 0;
        void FinishLoading(const Assets::AssetMetaData& assetMeta);

    private:
        virtual void OnSave(Assets::AssetSystem& assetSystem, const Assets::AssetMetaData& assetMeta) = 0;
        virtual void OnLoad(Assets::AssetSystem& assetSystem, const FileSystem::Filepath& path) = 0;

        virtual void OnDrawNode(const Node& node) = 0;
        virtual void OnDrawNodeBackground(const Node& node) = 0;
        virtual void OnNodeChanged(const Node& newNode);

        virtual bool OnNodeCreate(Node& newNode, StringId32 typeId) = 0;
        virtual void OnNodeDelete(Node& nodeToDelete) = 0;
        virtual void OnLinkCreate(const Link& newLink) = 0;
        virtual void OnLinkDelete(const Link& linkToDelete) = 0;

    private:
        DynamicArray<Node> m_Nodes;
        DynamicArray<Link> m_Links;

        Atomic<bool> m_IsLoading = false;
        const Localization::LocalizationModule* m_LocalizationModule = nullptr;
    };
}
