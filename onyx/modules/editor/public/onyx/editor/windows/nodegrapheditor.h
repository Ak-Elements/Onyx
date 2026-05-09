#pragma once

#include <onyx/ui/imguiwindow.h>

#include <onyx/editor/nodegraph/grapheditorcontext.h>
#include <onyx/ui/controls/dockspace.h>

namespace ax::NodeEditor {
struct EditorContext;
}

namespace onyx::localization {
class LocalizationModule;
}

namespace onyx::input_actions {
class InputActionSystem;
struct InputActionEvent;
} // namespace onyx::input_actions

namespace onyx::editor {
struct BlueprintNodeBuilder;

class NodeGraphEditorWindow : public ui::ImGuiWindow {
  public:
    static constexpr StringView WindowId = "NodeGraphEditor";
    static constexpr StringView WindowCategory = "Window";

    NodeGraphEditorWindow();
    ~NodeGraphEditorWindow() override;

    StringView getWindowId() override { return WindowId; }

    template < typename T >
    void setContext( T&& context ) {
        m_editorContext = std::forward< T >( context );
        m_editorContext->setLocalizationModule( getEngineSystem< localization::LocalizationModule >() );
        m_editorContext->OnLoaded.Connect< &NodeGraphEditorWindow::onGraphLoaded >( this );
        m_editorContext->OnSaved.Connect< &NodeGraphEditorWindow::onGraphSaved >( this );
        m_editorContext->LoadEditorMetaDataFunctor.Connect< &NodeGraphEditorWindow::loadEditorMetaData >( this );
        m_editorContext->SaveEditorMetaDataFunctor.Connect< &NodeGraphEditorWindow::saveEditorMetaData >( this );
        m_editorContext->OnNodeCreated.Connect< &NodeGraphEditorWindow::onNodeCreated >( this );
        m_shouldFocus = true;
        m_focusDuration = 0.0f;
    }

  private:
    void onOpen() override;
    void onClose() override;

    void onRender( ui::ImGuiSystem& imguiSystem ) override;

    void onRenderMainMenuBar() override;

  private:
    struct RerouteNode {
        RerouteNode() = default;
        RerouteNode( node_graph::PinTypeId pinTypeId, uint32_t pinTypeColor );

        Guid64 Id;

        Guid64 InputPinId;
        Guid64 OutputPinId;

        Guid64 InteractionPinId; // Pin to connect and create connections
        GraphEditorContext::PinDirection ActivePinDirection{ GraphEditorContext::PinDirection::Invalid };

        node_graph::PinTypeId PinTypeId{ node_graph::PinTypeId::Invalid };
        uint32_t Color = 0;

        Vector2f32 Position;
        bool HasUpdatedPosition = false;
    };

    struct RerouteLink {
        Guid64 Id;

        Guid64 FromInputPinId;
        Guid64 ToOutputPinId;

        node_graph::PinTypeId PinTypeId{ node_graph::PinTypeId::Invalid };
        uint32_t Color = 0;
    };

    void onUpdate( uint64_t deltaTime );

    void drawCanvas();

    void drawNode( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder );
    void drawNodeHeader( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder );
    void drawNodeInputs( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder );
    void drawNodeOutputs( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder );

    void drawRerouteNode( RerouteNode& node );

    void drawNodeLinks() const;

    void drawContextMenu();

    void drawCreateLink();
    void drawCreateNode();

    void drawPropertiesPanel();

    void filterNodeListContextMenu( StringView searchString );

    void save();
    void saveEditorMetaData( const FilePath& path );
    void load();
    void loadEditorMetaData( const FilePath& path );

    void onGraphLoaded();
    void onGraphSaved();

    void onCopyAction( const input_actions::InputActionEvent& inputActionContext );
    void onPasteAction( const input_actions::InputActionEvent& inputActionContext );
    void onDeleteAction( const input_actions::InputActionEvent& inputActionContext );

    void onNodeCreated( const GraphEditorContext::Node& node );

    void onLinkDoubleClicked( Guid64 linkId );

    ONYX_NO_DISCARD const RerouteNode* getRerouteNodeById( Guid64 nodeId ) const;
    ONYX_NO_DISCARD const RerouteNode* getRerouteNodeByPinId( Guid64 pinId ) const;

    void findRerouteDestinations( Guid64 reroutePinId, DynamicArray< Guid64 >& outDestinationPinIds );

  private:
    struct CreateNewNodeData {
        Guid64 PinId;
        node_graph::PinTypeId PinTypeId;
        GraphEditorContext::PinDirection Direction;
    };

    CreateNewNodeData m_createNodeData;

    ax::NodeEditor::EditorContext* m_context = nullptr;
    UniquePtr< GraphEditorContext > m_editorContext;

    DynamicArray< RerouteNode > m_rerouteNodes;
    DynamicArray< RerouteLink > m_rerouteLinks;

    float32 m_focusDuration = 0.0f;

    bool m_shouldFocus = false;
    bool m_showLinkDirections = false;

    GraphEditorContext::PinDirection m_forcedReroutePinDirection{ GraphEditorContext::PinDirection::Invalid };

    String m_canvasPanelId;
    String m_propertiesPanelId;

    uint32_t m_windowId;
};
} // namespace onyx::editor
