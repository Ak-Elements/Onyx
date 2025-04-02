#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/assets/asset.h>
#include <onyx/input/inputactionsystem.h>
#include <editor/nodegraph/grapheditorcontext.h>
#include <onyx/ui/controls/dockspace.h>

namespace ax::NodeEditor
{
    struct EditorContext;
}

namespace Onyx::Editor
{
    struct BlueprintNodeBuilder;

    class NodeGraphEditorWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "NodeGraphEditor";
        NodeGraphEditorWindow(Assets::AssetSystem& assetSystem, Input::InputActionSystem& inputActionSystem);
        ~NodeGraphEditorWindow() override;

        StringView GetWindowId() override { return WindowId; }

        template <typename T>
        void SetContext(T&& context)
        {
            m_EditorContext = std::forward<T>(context);
            m_EditorContext->OnLoaded.Connect<&NodeGraphEditorWindow::OnGraphLoaded>(this);
            m_EditorContext->OnSaved.Connect<&NodeGraphEditorWindow::OnGraphSaved>(this);
            m_EditorContext->LoadEditorMetaDataFunctor.Connect<&NodeGraphEditorWindow::LoadEditorMetaData>(this);
            m_EditorContext->SaveEditorMetaDataFunctor.Connect<&NodeGraphEditorWindow::SaveEditorMetaData>(this);

            m_ShouldFocus = true;
            m_FocusDuration = 0.0f;
        }

    private:
        void OnOpen() override;
        void OnClose() override;

        void OnRender(Ui::ImGuiSystem& system) override;
        void RenderMenuBar();

    private:
        struct RerouteNode
        {
            RerouteNode() = default;
            RerouteNode(NodeGraph::PinTypeId pinTypeId, onyxU32 pinTypeColor);

            Guid64 Id;

            Guid64 InputPinId;
            Guid64 OutputPinId;

            Guid64 InteractionPinId; // Pin to connect and create connections
            GraphEditorContext::PinDirection ActivePinDirection;

            NodeGraph::PinTypeId PinTypeId;
            onyxU32 Color;

            Vector2f Position;
            bool HasUpdatedPosition;
        };

        struct RerouteLink
        {
            Guid64 Id;

            Guid64 FromInputPinId;
            Guid64 ToOutputPinId;

            NodeGraph::PinTypeId PinTypeId;
            onyxU32 Color;
        };

        void OnUpdate(onyxU64 deltaTime);

        void DrawCanvas();

        void DrawNode(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder);
        void DrawNodeHeader(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder);
        void DrawNodeInputs(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder);
        void DrawNodeOutputs(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder);

        void DrawRerouteNode(RerouteNode& node);

        void DrawNodeLinks() const;

        void DrawContextMenu();
        bool DrawCanvasContextMenuNode(const GraphEditorContext::NodeListContextMenuItem& node, bool shouldForceOpen);

        void DrawCreateLink();
        void DrawCreateNode();

        void DrawPropertiesPanel();

        void FilterNodeListContextMenu(const StringView& searchString);
        void CreateNewNode(const GraphEditorContext::NodeListContextMenuItem& node);

        void Save();
        void SaveEditorMetaData(const FileSystem::Filepath& path);
        void Load();
        void LoadEditorMetaData(const FileSystem::Filepath& path);

        void OnGraphLoaded();
        void OnGraphSaved();

        void OnCopyAction(const Input::InputActionEvent& inputActionContext);
        void OnPasteAction(const Input::InputActionEvent& inputActionContext);
        void OnDeleteAction(const Input::InputActionEvent& inputActionContext);

        void OnLinkDoubleClicked(Guid64 linkId);

        const RerouteNode* GetRerouteNodeById(Guid64 nodeId) const;
        const RerouteNode* GetRerouteNodeByPinId(Guid64 pinId) const;

        void FindRerouteDestinations(Guid64 reroutePinId, DynamicArray<Guid64>& outDestinationPinIds);
    private:
        struct CreateNewNodeData
        {
            Guid64 PinId;
            NodeGraph::PinTypeId PinTypeId;
            GraphEditorContext::PinDirection Direction;
        };

        CreateNewNodeData m_CreateNodeData;
        
        Assets::AssetSystem& m_AssetSystem;
        Input::InputActionSystem& m_InputActionSystem;

        Assets::AssetId m_AssetId;

        ax::NodeEditor::EditorContext* m_Context = nullptr;
        UniquePtr<GraphEditorContext> m_EditorContext;

        Ui::Dockspace dockspace;

        DynamicArray<RerouteNode> m_RerouteNodes;
        DynamicArray<RerouteLink> m_RerouteLinks;

        onyxF32 m_FocusDuration = 0.0f;

        bool m_ShouldFocus = false;
        bool m_ShowLinkDirections = false;

        GraphEditorContext::PinDirection m_ForcedReroutePinDirection;
    };
}
