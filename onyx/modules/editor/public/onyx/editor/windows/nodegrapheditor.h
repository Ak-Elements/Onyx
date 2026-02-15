#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/editor/nodegraph/grapheditorcontext.h>
#include <onyx/ui/controls/dockspace.h>

namespace ax::NodeEditor
{
    struct EditorContext;
}

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::InputActions
{
    class InputActionSystem;
    struct InputActionEvent;
}

namespace Onyx::Editor
{
    struct BlueprintNodeBuilder;

    class NodeGraphEditorWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "NodeGraphEditor";
        static constexpr StringView WindowCategory = "Window";

        NodeGraphEditorWindow();
        ~NodeGraphEditorWindow() override;

        StringView GetWindowId() override { return WindowId; }

        template <typename T>
        void SetContext(T&& context)
        {
            m_EditorContext = std::forward<T>(context);
            m_EditorContext->SetLocalizationModule(GetEngineSystem<Localization::LocalizationModule>());
            m_EditorContext->OnLoaded.Connect<&NodeGraphEditorWindow::OnGraphLoaded>(this);
            m_EditorContext->OnSaved.Connect<&NodeGraphEditorWindow::OnGraphSaved>(this);
            m_EditorContext->LoadEditorMetaDataFunctor.Connect<&NodeGraphEditorWindow::LoadEditorMetaData>(this);
            m_EditorContext->SaveEditorMetaDataFunctor.Connect<&NodeGraphEditorWindow::SaveEditorMetaData>(this);
            m_EditorContext->OnNodeCreated.Connect<&NodeGraphEditorWindow::OnNodeCreated>(this);
            m_ShouldFocus = true;
            m_FocusDuration = 0.0f;
        }

    private:
        void OnOpen() override;
        void OnClose() override;

        void OnRender(Ui::ImGuiSystem& imguiSystem) override;
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

            Vector2f32 Position;
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

        void DrawCreateLink();
        void DrawCreateNode();

        void DrawPropertiesPanel();

        void FilterNodeListContextMenu(StringView searchString);

        void Save();
        void SaveEditorMetaData(const FilePath& path);
        void Load();
        void LoadEditorMetaData(const FilePath& path);

        void OnGraphLoaded();
        void OnGraphSaved();

        void OnCopyAction(const InputActions::InputActionEvent& inputActionContext);
        void OnPasteAction(const InputActions::InputActionEvent& inputActionContext);
        void OnDeleteAction(const InputActions::InputActionEvent& inputActionContext);

        void OnNodeCreated(const GraphEditorContext::Node& node);

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

        ax::NodeEditor::EditorContext* m_Context = nullptr;
        UniquePtr<GraphEditorContext> m_EditorContext;

        Ui::Dockspace m_Dockspace;

        DynamicArray<RerouteNode> m_RerouteNodes;
        DynamicArray<RerouteLink> m_RerouteLinks;

        onyxF32 m_FocusDuration = 0.0f;

        bool m_ShouldFocus = false;
        bool m_ShowLinkDirections = false;

        GraphEditorContext::PinDirection m_ForcedReroutePinDirection;

        String m_CanvasPanelId;
        String m_PropertiesPanelId;

        onyxU32 m_WindowId;
    };
}
