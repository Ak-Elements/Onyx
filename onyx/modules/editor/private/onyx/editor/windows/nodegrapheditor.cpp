#include <onyx/editor/windows/nodegrapheditor.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/editor/panels/nodeeditor/styles/blueprint.h>

#include <onyx/editor/nodegraph/nodegrapheditorcontext.h>
#include <onyx/editor/panels/nodeeditor/styles/widgets.h>

#include <onyx/editor/windows/editormainwindow.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/filesystem/filedialog.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/widgets.h>

#include <onyx/ui/imguisystem.h>

#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <onyx/editor/editor_localization.h>
#include <onyx/filesystem/onyxfile.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>

#include <onyx/inputactions/inputactionsystem.h>

namespace onyx::editor {
namespace {
constexpr float32 PIN_ICON_SIZE = 24.0f;

void DrawPinIcon( uint32_t color,
                  node_graph::PinTypeId typeId,
                  bool connected,
                  bool showTriangleTip,
                  ImVec2 hitbox,
                  int alpha ) {
    IconType iconType = IconType::Circle;
    ImColor imColor = color;
    imColor.Value.w = alpha / 255.0f;

    if ( typeId == node_graph::PinTypeId::Execute )
        iconType = IconType::Flow;

    Icon( ImVec2( PIN_ICON_SIZE, PIN_ICON_SIZE ),
          hitbox,
          iconType,
          connected,
          showTriangleTip,
          imColor,
          ImColor( 32, 32, 32, alpha ) );
};

uint32_t CreateHighlightColor( uint32_t color, float brightnessFactor ) {
    // Extract RGBA components (assuming color is in RGBA format)
    unsigned char r = ( color >> IM_COL32_R_SHIFT ) & 0xFF;
    unsigned char g = ( color >> IM_COL32_G_SHIFT ) & 0xFF;
    unsigned char b = ( color >> IM_COL32_B_SHIFT ) & 0xFF;
    unsigned char a = ( color >> IM_COL32_A_SHIFT ) & 0xFF;

    // Increase brightness
    r = static_cast< unsigned char >( std::min( r * brightnessFactor, 255.0f ) );
    g = static_cast< unsigned char >( std::min( g * brightnessFactor, 255.0f ) );
    b = static_cast< unsigned char >( std::min( b * brightnessFactor, 255.0f ) );

    // Recombine the color
    return IM_COL32( r, g, b, a );
}

bool local_IsCreatingNode = false;
uint32_t local_WindowId = 0;
} // namespace

NodeGraphEditorWindow::NodeGraphEditorWindow()
    : m_WindowId( local_WindowId++ ) {}

NodeGraphEditorWindow::~NodeGraphEditorWindow() = default;

void NodeGraphEditorWindow::OnOpen() {
    SetName( format::format( "Node Editor###NodeEditor{}", localization::editor::NodeEditor::Title, m_WindowId ) );
    m_CanvasPanelId = format::format( "###CanvasPanel{}", m_WindowId );
    m_PropertiesPanelId = format::format( "###PropertiesPanel{}", m_WindowId );

    ImGuiID dockspaceID = ImGui::GetID( format::format( "NodeEditor{}", m_WindowId ) );

    // TODO: should those be somewhere defined as default sizes?
    float windowWidth = ImGui::GetMainViewport()->Size.x;
    float propertiesPanelRatio = ( 400.0f / windowWidth );

    m_Dockspace = ui::Dockspace::Create(
        { { ui::DockSplitDirection::Right, propertiesPanelRatio, m_PropertiesPanelId, m_CanvasPanelId } } );
    m_Dockspace.SetId( dockspaceID );
    m_Dockspace.SetWindowClass( GetWindowClass() );

    ax::NodeEditor::Config config;
    // disable automatic save - We have to override SaveSettings to avoid a leak if the settings file is nullptr
    config.SettingsFile = nullptr;
    config.SaveSettings = []( const char*, size_t, ax::NodeEditor::SaveReasonFlags, void* ) { return true; };

    m_Context = ax::NodeEditor::CreateEditor( &config );

    input_actions::InputActionSystem& inputActionSystem = GetEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.OnInput< &NodeGraphEditorWindow::OnCopyAction >( "Copy"_id64, this );
    inputActionSystem.OnInput< &NodeGraphEditorWindow::OnPasteAction >( "Paste"_id64, this );
    inputActionSystem.OnInput< &NodeGraphEditorWindow::OnDeleteAction >( "Delete"_id64, this );
}

void NodeGraphEditorWindow::OnClose() {
    ax::NodeEditor::DestroyEditor( m_Context );
    m_Context = nullptr;
    m_EditorContext.reset();

    m_RerouteNodes.clear();
    m_RerouteLinks.clear();

    input_actions::InputActionSystem& inputActionSystem = GetEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );
}

void NodeGraphEditorWindow::OnRender( ui::ImGuiSystem& imguiSystem ) {
    if ( ( m_Context == nullptr ) || ( m_EditorContext == nullptr ) ) {
        return;
    }

    using namespace ax;

    if ( IsDocked() ) {
        SetWindowFlags( ImGuiWindowFlags_NoScrollWithMouse );
    } else {
        SetWindowFlags( ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar );
    }

    Optional< EditorMainWindow* > mainWindowOptional = imguiSystem.GetWindow< EditorMainWindow >();
    if ( mainWindowOptional.has_value() ) {
        EditorMainWindow& mainWindow = *mainWindowOptional.value();
        ImGui::SetNextWindowDockID( mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver );
    }

    // combine window name with graph name as the visual name
    if ( Begin() ) {
        if ( ImGui::IsWindowAppearing() ) {
            ImGui::BringWindowToDisplayFront( ImGui::GetCurrentWindow() );
        }

        RenderMenuBar();

        m_Dockspace.Render();

        ImGui::Begin( format::format( "{}{}", localization::editor::NodeEditor::UnnamedGraph, m_CanvasPanelId ) );

        NodeEditor::SetCurrentEditor( m_Context );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_Bg,
                                    ImGui::ColorConvertU32ToFloat4( m_EditorContext->GetCanvasBackgroundColor() ) );
        NodeEditor::Begin( "canvas" );

        if ( m_EditorContext->IsLoading() == false ) {
            DrawCanvas();
        }

        NodeEditor::End();

        ImGui::End();

        if ( m_EditorContext->IsLoading() == false ) {
            DrawPropertiesPanel();
        }

        if ( m_ShouldFocus ) {
            m_ShouldFocus = false;
            NodeEditor::NavigateToContent( false );
        }

        NodeEditor::SetCurrentEditor( nullptr );

    } else {
        m_Dockspace.Render();
    }

    End();
}

void NodeGraphEditorWindow::RenderMenuBar() {
    BeginMenuBar();

    if ( ImGui::BeginMenu( format::format( "{}###File", localization::generic::File ) ) ) {
        if ( ImGui::MenuItem( format::format( "{}###Open", localization::generic::Open ) ) ) {
            Load();
        }

        if ( ImGui::MenuItem( format::format( "{}###Save", localization::generic::Save ) ) ) {
            ax::NodeEditor::SetCurrentEditor( m_Context );
            Save();
            ax::NodeEditor::SetCurrentEditor( nullptr );
        }

        if ( ImGui::MenuItem( format::format( "{}###SaveAs", localization::generic::SaveAs ) ) ) {
            ax::NodeEditor::SetCurrentEditor( m_Context );
            Save();
            ax::NodeEditor::SetCurrentEditor( nullptr );
        }

        ImGui::EndMenu();
    }

    if ( ImGui::BeginMenu(
             format::format( "{}###Debug", localization::editor::NodeEditor::MainMenubar::Debug::Label ) ) ) {
        if ( ImGui::MenuItem(
                 format::format( "{}###ShowLinkDirections",
                                 localization::editor::NodeEditor::MainMenubar::Debug::ShowLinkDirections ),
                 0,
                 m_ShowLinkDirections ) ) {
            m_ShowLinkDirections = !m_ShowLinkDirections;
        }

        ImGui::EndMenu();
    }

    EndMenuBar();
}

NodeGraphEditorWindow::RerouteNode::RerouteNode( node_graph::PinTypeId pinTypeId, uint32_t pinTypeColor ) {
    Id = Guid64Generator::getGuid();
    InputPinId = Guid64Generator::getGuid();
    OutputPinId = Guid64Generator::getGuid();
    InteractionPinId = Guid64Generator::getGuid();

    PinTypeId = pinTypeId;
    Color = pinTypeColor;
}

void NodeGraphEditorWindow::DrawContextMenu() {
    using namespace ax;

    NodeEditor::Suspend();

    NodeEditor::LinkId linkId = 0;
    // if (ed::ShowNodeContextMenu(&contextNodeId))
    //     ImGui::OpenPopup("Node Context Menu");
    // else if (ed::ShowPinContextMenu(&contextPinId))
    //     ImGui::OpenPopup("Pin Context Menu");
    // else
    // NodeEditor::getlin
    if ( NodeEditor::ShowLinkContextMenu( &linkId ) ) {
        ImGui::OpenPopup( "Link Context Menu" );
    } else if ( NodeEditor::ShowBackgroundContextMenu() ) {
        ImGui::OpenPopup( "Create New Node" );
    }
    NodeEditor::Resume();

    ax::NodeEditor::Suspend();

    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4.0f, 4.0f ) );
    if ( ImGui::BeginPopup( "Create New Node" ) ) {
        static String s_SearchString;
        static bool s_HasFocus = false;

        const bool isAppearing = ImGui::IsWindowAppearing();
        s_HasFocus |= isAppearing;

        bool hasChanged = false;
        if ( isAppearing ) {
            s_SearchString.clear();
            hasChanged = true;
        }

        hasChanged |= ui::DrawSearchBar( s_SearchString, localization::generic::Search.Get(), s_HasFocus );
        if ( hasChanged ) {
            if ( s_SearchString.empty() && ( m_CreateNodeData.PinId.isValid() == false ) )
                m_EditorContext->ClearNodeListFilter();
            else
                FilterNodeListContextMenu( s_SearchString );
        }

        if ( ImGui::BeginChild( "##NodesScrollList",
                                ImVec2( 350.0f, 350.0f ),
                                ImGuiChildFlags_AlwaysUseWindowPadding ) ) {
            // TODO: Currently is we have a pin filter we force open all the time - we should fix thaxt to only force
            // open once
            const ui::TreeItem& nodeListMenuRoot = m_EditorContext->GetNodeListContextMenuRoot();
            ui::TreeViewFlags flags = isAppearing ? ui::TreeViewFlags::ForceCloseAll
                                                  : ( s_SearchString.empty() ? ui::TreeViewFlags::None
                                                                             : ui::TreeViewFlags::ForceOpenAll );
            bool hasClickedItem = ui::RenderTreeView( "CreateNodeMenu", nodeListMenuRoot, flags );
            if ( hasClickedItem ) {
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndChild();

        ImGui::EndPopup();
    } else {
        m_CreateNodeData.PinId.reset();
        m_CreateNodeData.PinTypeId = node_graph::PinTypeId::Invalid;

        local_IsCreatingNode = false;
    }

    ImGui::PopStyleVar();

    NodeEditor::Resume();
}

void NodeGraphEditorWindow::OnLinkDoubleClicked( Guid64 linkId ) {
    Guid64 fromPinId;
    Guid64 toPinId;

    node_graph::PinTypeId pinTypeId = node_graph::PinTypeId::Invalid;
    uint32_t pinTypeColor = 0;

    auto rerouteLinkIt = std::ranges::find_if( m_RerouteLinks,
                                               [ & ]( RerouteLink& link ) { return link.Id == linkId; } );

    if ( rerouteLinkIt == m_RerouteLinks.end() ) {
        auto it = std::ranges::find_if( m_EditorContext->GetLinks(),
                                        [ & ]( GraphEditorContext::Link& link ) { return link.Id == linkId; } );

        if ( it != m_EditorContext->GetLinks().end() ) {
            fromPinId = it->FromPinId;
            toPinId = it->ToPinId;
            pinTypeColor = it->Color;
            pinTypeId = m_EditorContext->GetPin( it->FromPinId ).PinTypeId;

            it->IsRerouted = true;
        }
    } else {
        fromPinId = rerouteLinkIt->FromInputPinId;
        toPinId = rerouteLinkIt->ToOutputPinId;

        pinTypeId = rerouteLinkIt->PinTypeId;
        pinTypeColor = rerouteLinkIt->Color;

        m_RerouteLinks.erase( rerouteLinkIt );
    }

    if ( fromPinId.isValid() && toPinId.isValid() ) {
        const ImVec2 newNodePostion = ImGui::GetMousePos();
        RerouteNode& newRerouteNode = m_RerouteNodes.emplace_back( pinTypeId, pinTypeColor );
        ax::NodeEditor::SetNodePosition( newRerouteNode.Id.get(), newNodePostion );

        m_RerouteLinks.emplace_back( Guid64Generator::getGuid(),
                                     newRerouteNode.InputPinId,
                                     toPinId,
                                     pinTypeId,
                                     pinTypeColor );
        m_RerouteLinks.emplace_back( Guid64Generator::getGuid(),
                                     fromPinId,
                                     newRerouteNode.OutputPinId,
                                     pinTypeId,
                                     pinTypeColor );
    }
}

const NodeGraphEditorWindow::RerouteNode* NodeGraphEditorWindow::GetRerouteNodeById( Guid64 nodeId ) const {
    for ( const RerouteNode& node : m_RerouteNodes ) {
        if ( node.Id == nodeId )
            return &node;
    }

    return nullptr;
}

const NodeGraphEditorWindow::RerouteNode* NodeGraphEditorWindow::GetRerouteNodeByPinId( Guid64 pinId ) const {
    for ( const RerouteNode& node : m_RerouteNodes ) {
        if ( node.InputPinId == pinId )
            return &node;

        if ( node.OutputPinId == pinId )
            return &node;

        if ( node.InteractionPinId == pinId )
            return &node;
    }

    return nullptr;
}

void NodeGraphEditorWindow::FindRerouteDestinations( Guid64 reroutePinId,
                                                     DynamicArray< Guid64 >& outDestinationPinIds ) {
    const RerouteNode* rerouteNode = GetRerouteNodeByPinId( reroutePinId );
    if ( rerouteNode == nullptr ) {
        outDestinationPinIds.push_back( reroutePinId );
        return;
    }

    bool isWalkingBackwards = ( rerouteNode->InputPinId != reroutePinId );

    Stack< Guid64 > traversalStack;
    traversalStack.push( isWalkingBackwards ? rerouteNode->InputPinId : rerouteNode->OutputPinId );

    while ( traversalStack.empty() == false ) {
        Guid64 currentPinId = traversalStack.top();
        traversalStack.pop();

        for ( const RerouteLink& link : m_RerouteLinks ) {
            Guid64 linkedPinId;
            if ( link.ToOutputPinId == currentPinId )
                linkedPinId = link.FromInputPinId;
            else if ( link.FromInputPinId == currentPinId )
                linkedPinId = link.ToOutputPinId;
            else
                continue;

            rerouteNode = GetRerouteNodeByPinId( linkedPinId );
            // if we failed to get a reroute node for the pin id, this pin has to be a real node pin
            if ( rerouteNode == nullptr )
                outDestinationPinIds.push_back( linkedPinId );
            else
                traversalStack.push( isWalkingBackwards ? rerouteNode->InputPinId : rerouteNode->OutputPinId );
        }
    }
}

void NodeGraphEditorWindow::DrawRerouteNode( RerouteNode& node ) {
    constexpr float32 PIN_ICON_SIZE_HALF = PIN_ICON_SIZE / 2.0f;
    constexpr float32 PIN_ICON_SIZE_QUATER = PIN_ICON_SIZE / 4.0f;

    constexpr ImVec2 PIN_ICON_SIZE_HALF_2D( PIN_ICON_SIZE_HALF, PIN_ICON_SIZE_HALF );
    constexpr ImVec2 INTERACTION_PIN_OFFSET( PIN_ICON_SIZE_QUATER, PIN_ICON_SIZE_QUATER );
    constexpr ImVec2 HIDDEN_PIN_SIZE( 0.1f, 0.1f );

    ImVec2 mousePos = ImGui::GetMousePos();

    Guid64 inputPinId = node.InputPinId;
    Guid64 outputPinId = node.OutputPinId;

    ax::NodeEditor::PinId hoveredPinId = ax::NodeEditor::GetHoveredPin();
    bool isPinHovered = ( hoveredPinId.Get() == node.InteractionPinId.get() );

    ax::NodeEditor::PushStyleVar( ax::NodeEditor::StyleVar_NodePadding, ImVec4( 10, 4, 10, 4 ) );
    ax::NodeEditor::PushStyleVar( ax::NodeEditor::StyleVar_NodeBorderWidth, 0.0f );
    ax::NodeEditor::PushStyleColor( ax::NodeEditor::StyleColor_NodeBg, ImVec4( 0, 0, 0, 0 ) );
    if ( isPinHovered ) {
        ax::NodeEditor::PushStyleColor( ax::NodeEditor::StyleColor_HovNodeBorder, ImVec4( 0, 0, 0, 0 ) );
    }

    ax::NodeEditor::BeginNode( node.Id.get() );

    {
        // Input pin
        // Overlapping Input and Output Pins
        ImVec2 pinPosition = ImGui::GetCursorPos(); // Adjust as needed
        ImVec2 hiddenPinMinPosition = pinPosition + PIN_ICON_SIZE_HALF_2D - HIDDEN_PIN_SIZE;
        ImVec2 hiddenPinMaxPosition = pinPosition + PIN_ICON_SIZE_HALF_2D + HIDDEN_PIN_SIZE;

        ImGui::SetNextItemAllowOverlap();
        ImGui::Dummy( ImVec2( PIN_ICON_SIZE, PIN_ICON_SIZE ) );

        // Input pin
        ImGui::SetNextItemAllowOverlap();
        ImGui::SetCursorPos( pinPosition );
        ax::NodeEditor::BeginPin( inputPinId.get(), ax::NodeEditor::PinKind::Input );
        ax::NodeEditor::PinPivotAlignment( ImVec2( 0.5f, 0.5f ) ); // Align the pivot to the center of the area
        ax::NodeEditor::PinRect( hiddenPinMinPosition, hiddenPinMaxPosition );
        ax::NodeEditor::EndPin();

        // Output pin (shares the same pivot rect as the input pin)
        ImGui::SetNextItemAllowOverlap();

        ImGui::SetCursorPos( pinPosition );
        ax::NodeEditor::BeginPin( outputPinId.get(), ax::NodeEditor::PinKind::Output );
        ax::NodeEditor::PinPivotAlignment( ImVec2( 0.5f, 0.5f ) ); // Align the pivot to the center of the area
        ax::NodeEditor::PinRect( hiddenPinMinPosition, hiddenPinMaxPosition );
        ax::NodeEditor::EndPin();

        ax::NodeEditor::PinKind pinKind = ax::NodeEditor::PinKind::Input;
        node.ActivePinDirection = GraphEditorContext::PinDirection::Input;

        if ( m_ForcedReroutePinDirection != GraphEditorContext::PinDirection::Invalid ) {
            node.ActivePinDirection = m_ForcedReroutePinDirection;
            pinKind = m_ForcedReroutePinDirection == GraphEditorContext::PinDirection::Input
                          ? ax::NodeEditor::PinKind::Input
                          : ax::NodeEditor::PinKind::Output;
        } else {
            if ( mousePos.x >= ( pinPosition.x + PIN_ICON_SIZE_HALF_2D.x ) ) {
                pinKind = ax::NodeEditor::PinKind::Output;
                node.ActivePinDirection = GraphEditorContext::PinDirection::Output;
            }
        }

        ImGui::SetCursorPos( pinPosition );

        ax::NodeEditor::PushStyleColor( ax::NodeEditor::StyleColor_PinRect, ImVec4( 0, 0, 0, 0 ) );
        ax::NodeEditor::BeginPin( node.InteractionPinId.get(), pinKind );
        ax::NodeEditor::PinPivotAlignment( ImVec2( 0.5f, 0.5f ) ); // Align the pivot to the center of the area
        ax::NodeEditor::PinRect( pinPosition + PIN_ICON_SIZE_HALF_2D - INTERACTION_PIN_OFFSET,
                                 pinPosition + PIN_ICON_SIZE_HALF_2D + INTERACTION_PIN_OFFSET );

        uint32_t pinColor = isPinHovered ? CreateHighlightColor( node.Color, 1.5f ) : node.Color;

        DrawPinIcon( pinColor, node.PinTypeId, true, false, PIN_ICON_SIZE_HALF_2D, 255 );

        ax::NodeEditor::EndPin();
        ax::NodeEditor::PopStyleColor();
    }

    ax::NodeEditor::EndNode();
    ax::NodeEditor::PopStyleVar( 2 );
    ax::NodeEditor::PopStyleColor( isPinHovered ? 2 : 1 );
}

void NodeGraphEditorWindow::DrawCanvas() {
    if ( m_EditorContext == nullptr )
        return;

    const ImVec2& cursorTopLeft = ImGui::GetCursorScreenPos();

    // Start drawing nodes.
    BlueprintNodeBuilder builder;
    DynamicArray< GraphEditorContext::Node >& nodes = m_EditorContext->GetNodes();
    for ( GraphEditorContext::Node& node : nodes ) {
        if ( node.HasUpdatedPosition ) {
            ax::NodeEditor::SetNodePosition( node.Id.get(), ImVec2( node.Position[ 0 ], node.Position[ 1 ] ) );
            node.HasUpdatedPosition = false;
        }

        DrawNode( node, builder );
    }

    for ( RerouteNode& node : m_RerouteNodes ) {
        if ( node.HasUpdatedPosition ) {
            ax::NodeEditor::SetNodePosition( node.Id.get(), ImVec2( node.Position[ 0 ], node.Position[ 1 ] ) );
            node.HasUpdatedPosition = false;
        }

        DrawRerouteNode( node );
    }

    // draw links
    DrawNodeLinks();

    if ( local_IsCreatingNode == false ) {
        // draw link create
        DrawCreateLink();
    }

    ImGui::SetCursorScreenPos( cursorTopLeft );

    DrawContextMenu();

    if ( ax::NodeEditor::LinkId linkId = ax::NodeEditor::GetDoubleClickedLink() ) {
        OnLinkDoubleClicked( Guid64( linkId.Get() ) );
    }
}

void NodeGraphEditorWindow::DrawPropertiesPanel() {
    using namespace ax;

    {
        ui::ScopedImGuiStyle dockspaceStyle{ { ImGuiStyleVar_FrameBorderSize, 0.0f },
                                             { ImGuiStyleVar_ItemSpacing, ImVec2( 0.0, 0.0f ) },
                                             { ImGuiStyleVar_ItemInnerSpacing, ImVec2( 0.0, 0.0f ) } };

        ImGui::Begin( format::format( "Properties{}", m_PropertiesPanelId ),
                      nullptr,
                      ImGuiWindowFlags_HorizontalScrollbar );
    }

    DynamicArray< NodeEditor::NodeId > selectedNodes;
    selectedNodes.resize( NodeEditor::GetSelectedObjectCount() );
    int nodeCount = NodeEditor::GetSelectedNodes( selectedNodes.data(), static_cast< int >( selectedNodes.size() ) );
    selectedNodes.resize( nodeCount );

    // TODO: Why is vertical group not working with property grid?

    ImGui::BeginVertical( "##SelectedNodeProperties" );

    // The property grid should allow editing the same property of multiple nodes at the same time if multiselected
    // e.g.: if 2 Add Floats are selected, you can edit the inputs of both with 1 input field
    // For that we have to collect all nodes with the same PinID

    for ( NodeEditor::NodeId nodeId : selectedNodes ) {
        Guid64 globalNodeId( nodeId.Get() );

        if ( GetRerouteNodeById( globalNodeId ) != nullptr )
            continue;

        ImGui::PushID( nodeId.AsPointer< uintptr_t >() );

        {
            ui::ScopedImGuiStyle nodePanelStyle{ { ImGuiStyleVar_FramePadding, ImVec2( 8.0f, 8.0f ) } };
            ImGui::BeginChild( "##Node",
                               ImVec2( 0, 0 ),
                               ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle );
        }

        {
            ui::ScopedImGuiStyle propGridStyle{ { ImGuiStyleVar_ItemSpacing, ImVec2( 4.0f, 4.0f ) } };

            ui::property_grid::beginPropertyGrid( "##propertyGrid", 120.0f );
            m_EditorContext->DrawNodeInPropertyPanel( globalNodeId );
            ui::property_grid::endPropertyGrid();
        }

        ImGui::EndChild();

        ImGui::PopID();
    }

    ImGui::EndVertical();

    ImGui::End();
}

void NodeGraphEditorWindow::FilterNodeListContextMenu( StringView searchString ) {
    m_EditorContext->FilterNodeListContextMenu( [ & ]( StringView localizedFullyQualifiedNodeName,
                                                       const node_graph::NodeEditorMetaData& nodeMetaData ) {
        if ( ignoreCaseFind( localizedFullyQualifiedNodeName, searchString ) == StringView::npos ) {
            if ( nodeMetaData.HasAliases == false ) {
                return false;
            }

            // look for any aliases
            localization::LocalizationModule&
                localizationSystem = GetEngineSystem< localization::LocalizationModule >();
            localization::LocalizationId aliasLocalizationId( "alias", nodeMetaData.TypeId );
            Optional< StringView > localizedAliasesOptional = localizationSystem.TryGetLocalized( aliasLocalizationId );
            if ( localizedAliasesOptional.has_value() ) {
                StringView localizedAliases = *localizedAliasesOptional;
                if ( ignoreCaseFind( localizedAliases, searchString ) == StringView::npos ) {
                    return false;
                }
            } else {
                ONYX_LOG_WARNING( "Node is marked to have aliases but is missing the localization entry for it. "
                                  "TypeId: {}, Localized name: {}",
                                  nodeMetaData.TypeId,
                                  localizedFullyQualifiedNodeName );
                return false;
            }
        }

        if ( m_CreateNodeData.PinTypeId != node_graph::PinTypeId::Invalid ) {
            if ( m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Input ) {
                for ( const node_graph::PinTypeId& outputType : nodeMetaData.OutputPins ) {
                    if ( outputType == m_CreateNodeData.PinTypeId )
                        return true;
                }

                return false;
            } else if ( m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Output ) {
                for ( const node_graph::PinTypeId& inputType : nodeMetaData.InputPins ) {
                    if ( inputType == m_CreateNodeData.PinTypeId )
                        return true;
                }

                return false;
            }
        }

        return true;
    } );
}

void NodeGraphEditorWindow::OnNodeCreated( const GraphEditorContext::Node& node ) {
    // if (m_cre)
    const ImVec2 newNodePostion = ax::NodeEditor::ScreenToCanvas( ImGui::GetMousePosOnOpeningCurrentPopup() );
    ax::NodeEditor::SetNodePosition( node.Id.get(), newNodePostion );

    if ( m_CreateNodeData.PinId.isValid() ) {
        // check if pin is a reroute pin and get original pin for the real graph connection
        Guid64 fromPin = m_CreateNodeData.PinId;
        Guid64 toPin = m_CreateNodeData.PinId;
        if ( m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Input ) {
            for ( const GraphEditorContext::Pin& outputPin : node.Outputs ) {
                if ( outputPin.PinTypeId == m_CreateNodeData.PinTypeId ) {
                    toPin = outputPin.Id;
                    break;
                }
            }
        } else if ( m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Output ) {
            for ( const GraphEditorContext::Pin& inputPin : node.Inputs ) {
                if ( inputPin.PinTypeId == m_CreateNodeData.PinTypeId ) {
                    fromPin = inputPin.Id;
                    break;
                }
            }
        }

        if ( m_CreateNodeData.PinTypeId == node_graph::PinTypeId::Execute ) {
            std::swap( fromPin, toPin );
        }

        const RerouteNode* rerouteNode = GetRerouteNodeByPinId( m_CreateNodeData.PinId );
        if ( rerouteNode == nullptr ) {
            m_EditorContext->CreateNewLink( fromPin, toPin );
        } else {
            DynamicArray< Guid64 > rerouteDestinationPinIds;
            FindRerouteDestinations( m_CreateNodeData.PinId, rerouteDestinationPinIds );

            // delete any existing link
            std::ignore = std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& rerouteLink ) {
                return rerouteLink.FromInputPinId == m_CreateNodeData.PinId;
            } );

            m_RerouteLinks.emplace_back( Guid64Generator::getGuid(),
                                         fromPin,
                                         toPin,
                                         m_CreateNodeData.PinTypeId,
                                         rerouteNode->Color );

            for ( Guid64 rerouteDestinationPinId : rerouteDestinationPinIds ) {
                if ( fromPin == m_CreateNodeData.PinId ) {
                    GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink( rerouteDestinationPinId,
                                                                                        toPin );
                    newLink.IsRerouted = true;
                } else {
                    GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink( fromPin,
                                                                                        rerouteDestinationPinId );
                    newLink.IsRerouted = true;
                }
            }
        }
    }

    local_IsCreatingNode = false;
    m_EditorContext->ClearNodeListFilter();
}

void NodeGraphEditorWindow::Save() {
    FilePath path;
    if ( file_system::FileDialog::SaveFileDialog( path,
                                                  m_EditorContext->GetLocalizedAssetTypeName(),
                                                  m_EditorContext->GetExtensions() ) ) {
        assets::AssetMetaData dummyAsset;
        dummyAsset.Path = path;
        dummyAsset.Id = assets::AssetId( dummyAsset.Path );

        if ( m_EditorContext->Compile() == false ) {
            return;
        }

        assets::AssetSystem& assetSystem = GetEngineSystem< assets::AssetSystem >();
        m_EditorContext->Save( assetSystem, dummyAsset );
    }
}

void NodeGraphEditorWindow::Load() {
    FilePath path;
    if ( file_system::FileDialog::OpenFileDialog( path,
                                                  m_EditorContext->GetLocalizedAssetTypeName(),
                                                  m_EditorContext->GetExtensions() ) ) {
        assets::AssetSystem& assetSystem = GetEngineSystem< assets::AssetSystem >();
        m_EditorContext->Load( assetSystem, path );
    }
}

void NodeGraphEditorWindow::SaveEditorMetaData( const FilePath& path ) {
    using namespace file_system;
    JsonValue jsonRoot;

    JsonValue nodesJsonArray;
    for ( const GraphEditorContext::Node& node : m_EditorContext->GetNodes() ) {
        JsonValue nodeMetaInfo;
        nodeMetaInfo.Set( "id", node.Id );
        nodeMetaInfo.Set( "name", node.Name );

        ImVec2 nodePosition = ax::NodeEditor::GetNodePosition( node.Id.get() );
        Array< float32, 2 > position{ nodePosition.x, nodePosition.y };
        nodeMetaInfo.Set( "position", position );

        nodesJsonArray.Add( nodeMetaInfo );
    }

    jsonRoot.Set( "nodes", nodesJsonArray );

    // this is added here to save the visibility state of links (for hidden links based on reroute nodes)
    JsonValue linksJsonArray;
    for ( const GraphEditorContext::Link& link : m_EditorContext->GetLinks() ) {
        if ( link.IsRerouted == false )
            continue;

        JsonValue linkMetaInfo;
        linkMetaInfo.Set( "frominputpin", link.FromPinId );
        linkMetaInfo.Set( "tooutputpin", link.ToPinId );

        linksJsonArray.Add( linkMetaInfo );
    }

    if ( linksJsonArray.Json.empty() == false )
        jsonRoot.Set( "hiddenlinks", linksJsonArray );

    if ( m_RerouteNodes.empty() == false ) {
        JsonValue rerouteNodesJsonArray;
        for ( const RerouteNode& node : m_RerouteNodes ) {
            JsonValue nodeMetaInfo;
            nodeMetaInfo.Set( "id", node.Id );
            nodeMetaInfo.Set( "inputpin", node.InputPinId );
            nodeMetaInfo.Set( "outputpin", node.OutputPinId );

            ImVec2 nodePosition = ax::NodeEditor::GetNodePosition( node.Id.get() );
            Array< float32, 2 > position{ nodePosition.x, nodePosition.y };
            nodeMetaInfo.Set( "position", position );

            rerouteNodesJsonArray.Add( nodeMetaInfo );
        }

        jsonRoot.Set( "reroutenodes", rerouteNodesJsonArray );
    }

    if ( m_RerouteLinks.empty() == false ) {
        JsonValue rerouteLinksJsonArray;
        for ( const RerouteLink& link : m_RerouteLinks ) {
            JsonValue linkMetaInfo;
            linkMetaInfo.Set( "id", link.Id );
            linkMetaInfo.Set( "frominputpin", link.FromInputPinId );
            linkMetaInfo.Set( "tooutputpin", link.ToOutputPinId );

            rerouteLinksJsonArray.Add( linkMetaInfo );
        }

        jsonRoot.Set( "reroutelinks", rerouteLinksJsonArray );
    }

    FilePath metaFilePath = Path::ReplaceExtension( path, "ometa" );
    OnyxFile metaDataFile( Path::GetFullPath( metaFilePath ) );
    FileStream stream = metaDataFile.OpenStream( OpenMode::Write | OpenMode::Text );

    const String& jsonString = jsonRoot.Json.dump( 4 );
    stream.writeRaw( jsonString.data(), jsonString.size() );
}

void NodeGraphEditorWindow::LoadEditorMetaData( const FilePath& path ) {
    // clear reroute nodes
    m_RerouteNodes.clear();
    m_RerouteLinks.clear();

    using namespace file_system;
    FilePath metaFilePath = file_system::Path::ReplaceExtension( path, "ometa" );
    OnyxFile metaDataJsonFile( file_system::Path::GetFullPath( metaFilePath ) );

    const JsonValue& metaDataJsonRoot = metaDataJsonFile.LoadJson();

    JsonValue nodesJsonArray;
    if ( metaDataJsonRoot.Get( "nodes", nodesJsonArray ) == false ) {
        // fallback to root if we don't find nodes array in json, this is to allow loading of old meta data
        nodesJsonArray.Json = metaDataJsonRoot.Json;
    }

    for ( const auto& nodeMetaJson : nodesJsonArray.Json ) {
        JsonValue nodeMetaJsonObj{ nodeMetaJson };

        Guid64 nodeId;
        String nodeName;
        nodeMetaJsonObj.Get( "id", nodeId );
        nodeMetaJsonObj.Get( "name", nodeName );

        Array< float32, 2 > position{};
        nodeMetaJsonObj.Get( "position", position );

        GraphEditorContext::Node& node = m_EditorContext->GetNode( nodeId );
        node.Name = nodeName;
        node.Position = Vector2f32( position[ 0 ], position[ 1 ] );
        node.HasUpdatedPosition = true;
    }

    JsonValue linksJsonArray;
    if ( metaDataJsonRoot.Get( "hiddenlinks", linksJsonArray ) ) {
        for ( const auto& linkMetaJson : linksJsonArray.Json ) {
            JsonValue linkMetaJsonObj{ linkMetaJson };

            Guid64 fromInputPinId;
            Guid64 toOutputPinId;
            linkMetaJsonObj.Get( "frominputpin", fromInputPinId );
            linkMetaJsonObj.Get( "tooutputpin", toOutputPinId );

            auto it = std::ranges::find_if( m_EditorContext->GetLinks(), [ & ]( const GraphEditorContext::Link& link ) {
                return ( link.FromPinId == fromInputPinId ) && ( link.ToPinId == toOutputPinId );
            } );

            if ( it == m_EditorContext->GetLinks().end() ) {
                ONYX_LOG_ERROR( "Missing link from node graph" );
                continue;
            }

            it->IsRerouted = true;
        }
    }

    JsonValue rerouteNodesJsonArray;
    if ( metaDataJsonRoot.Get( "reroutenodes", rerouteNodesJsonArray ) ) {
        for ( const auto& rerouteNodeMetaJson : rerouteNodesJsonArray.Json ) {
            JsonValue rerouteNodeMetaJsonObj{ rerouteNodeMetaJson };

            RerouteNode& newReroute = m_RerouteNodes.emplace_back();
            newReroute.InteractionPinId = Guid64Generator::getGuid();
            rerouteNodeMetaJsonObj.Get( "id", newReroute.Id );
            rerouteNodeMetaJsonObj.Get( "inputpin", newReroute.InputPinId );
            rerouteNodeMetaJsonObj.Get( "outputpin", newReroute.OutputPinId );

            rerouteNodeMetaJsonObj.Get( "color", newReroute.Color );

            Array< float32, 2 > position{};
            rerouteNodeMetaJsonObj.Get( "position", position );
            newReroute.Position = Vector2f32( position[ 0 ], position[ 1 ] );
            newReroute.HasUpdatedPosition = true;
        }
    }

    JsonValue rerouteLinksJsonArray;
    if ( metaDataJsonRoot.Get( "reroutelinks", rerouteLinksJsonArray ) ) {
        for ( const auto& rerouteLinkMetaJson : rerouteLinksJsonArray.Json ) {
            JsonValue rerouteLinkMetaJsonObj{ rerouteLinkMetaJson };

            RerouteLink& newLink = m_RerouteLinks.emplace_back();
            rerouteLinkMetaJsonObj.Get( "id", newLink.Id );
            rerouteLinkMetaJsonObj.Get( "frominputpin", newLink.FromInputPinId );
            rerouteLinkMetaJsonObj.Get( "tooutputpin", newLink.ToOutputPinId );

            // uint32_t pinTypeId;
            // rerouteLinkMetaJsonObj.Get("pintype", pinTypeId);
            // newLink.PinTypeId = enums::ToEnum<NodeGraph::PinTypeId>(pinTypeId);

            rerouteLinkMetaJsonObj.Get( "color", newLink.Color );
        }
    }

    // TODO: Fix this to make the retrieval smarter and maybe remove typeId and color from the reroute links
    for ( RerouteNode& node : m_RerouteNodes ) {
        DynamicArray< Guid64 > destinations;
        FindRerouteDestinations( node.InputPinId, destinations );

        if ( destinations.empty() == false ) {
            const GraphEditorContext::Pin& pin = m_EditorContext->GetPin( destinations.front() );
            node.PinTypeId = pin.PinTypeId;
            node.Color = pin.Color;
        } else {
            FindRerouteDestinations( node.OutputPinId, destinations );
            if ( destinations.empty() == false ) {
                const GraphEditorContext::Pin& pin = m_EditorContext->GetPin( destinations.front() );
                node.PinTypeId = pin.PinTypeId;
                node.Color = pin.Color;
            }
        }
    }

    for ( RerouteLink& link : m_RerouteLinks ) {
        DynamicArray< Guid64 > destinations;
        FindRerouteDestinations( link.FromInputPinId, destinations );

        if ( destinations.empty() == false ) {
            const GraphEditorContext::Pin& pin = m_EditorContext->GetPin( destinations.front() );
            link.PinTypeId = pin.PinTypeId;
            link.Color = pin.Color;
        } else {
            FindRerouteDestinations( link.ToOutputPinId, destinations );
            if ( destinations.empty() == false ) {
                const GraphEditorContext::Pin& pin = m_EditorContext->GetPin( destinations.front() );
                link.PinTypeId = pin.PinTypeId;
                link.Color = pin.Color;
            }
        }
    }
}

void NodeGraphEditorWindow::OnGraphLoaded() {
    m_ShouldFocus = true;
    m_FocusDuration = 0.0f;
}

void NodeGraphEditorWindow::OnGraphSaved() {}

void NodeGraphEditorWindow::OnCopyAction( const input_actions::InputActionEvent& inputActionContext ) {
    ONYX_UNUSED( inputActionContext );
}

void NodeGraphEditorWindow::OnPasteAction( const input_actions::InputActionEvent& inputActionContext ) {
    ONYX_UNUSED( inputActionContext );
}

void NodeGraphEditorWindow::OnDeleteAction( const input_actions::InputActionEvent& /*inputActionContext*/ ) {
    ax::NodeEditor::SetCurrentEditor( m_Context );
    DynamicArray< ax::NodeEditor::NodeId > selectedNodes;
    selectedNodes.resize( ax::NodeEditor::GetSelectedObjectCount() );
    int nodeCount = ax::NodeEditor::GetSelectedNodes( selectedNodes.data(),
                                                      static_cast< int >( selectedNodes.size() ) );
    selectedNodes.resize( nodeCount );

    for ( ax::NodeEditor::NodeId selectedNode : selectedNodes ) {
        Guid64 globalNodeId( selectedNode.Get() );
        if ( const RerouteNode* rerouteNode = GetRerouteNodeById( globalNodeId ) ) {
            for ( RerouteLink& inputLink : m_RerouteLinks ) {
                if ( inputLink.FromInputPinId != rerouteNode->InputPinId )
                    continue;

                for ( RerouteLink& outputLink : m_RerouteLinks ) {
                    if ( outputLink.ToOutputPinId != rerouteNode->OutputPinId )
                        continue;

                    outputLink.ToOutputPinId = inputLink.ToOutputPinId;
                }
            }

            std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& link ) {
                return ( link.FromInputPinId == rerouteNode->InputPinId ) ||
                       ( link.ToOutputPinId == rerouteNode->OutputPinId );
            } );

            std::erase_if( m_RerouteNodes, [ & ]( const RerouteNode& node ) { return node.Id == rerouteNode->Id; } );
        } else {
            // TODO: A remove link callback might be smarter than brute forcing this here
            const GraphEditorContext::Node& node = m_EditorContext->GetNode( globalNodeId );
            for ( const GraphEditorContext::Pin& inputPin : node.Inputs ) {
                std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& link ) {
                    return ( link.FromInputPinId == inputPin.Id ) || ( link.ToOutputPinId == inputPin.Id );
                } );
            }

            for ( const GraphEditorContext::Pin& outputPin : node.Outputs ) {
                std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& link ) {
                    return ( link.FromInputPinId == outputPin.Id ) || ( link.ToOutputPinId == outputPin.Id );
                } );
            }
        }

        m_EditorContext->DeleteNode( globalNodeId );
        ax::NodeEditor::DeleteNode( selectedNode );
    }

    ax::NodeEditor::ClearSelection();
    ax::NodeEditor::SetCurrentEditor( nullptr );
}

void NodeGraphEditorWindow::DrawNode( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder ) {
    builder.Begin( node.Id.get() );

    DrawNodeHeader( node, builder );
    DrawNodeInputs( node, builder );
    DrawNodeOutputs( node, builder );

    m_EditorContext->DrawNode( node );

    builder.End();

    // we have to draw the background after the node has ended.
    m_EditorContext->DrawNodeBackground( node );
}

void NodeGraphEditorWindow::DrawNodeLinks() const {
    for ( const RerouteLink& reroutedLink : m_RerouteLinks ) {
        const uint64_t linkId = reroutedLink.Id.get();
        ax::NodeEditor::Link( linkId,
                              reroutedLink.FromInputPinId.get(),
                              reroutedLink.ToOutputPinId.get(),
                              ImGui::ColorConvertU32ToFloat4( reroutedLink.Color ),
                              4 );

        if ( m_ShowLinkDirections )
            ax::NodeEditor::Flow( linkId, ax::NodeEditor::FlowDirection::Backward );
    }

    for ( const GraphEditorContext::Link& link : m_EditorContext->GetLinks() ) {
        if ( link.IsRerouted )
            continue;

        const uint64_t linkId = link.Id.get();
        ax::NodeEditor::Link( linkId,
                              link.FromPinId.get(),
                              link.ToPinId.get(),
                              ImGui::ColorConvertU32ToFloat4( link.Color ),
                              4 );

        if ( m_ShowLinkDirections )
            ax::NodeEditor::Flow( linkId, ax::NodeEditor::FlowDirection::Backward );
    }
}

void NodeGraphEditorWindow::DrawCreateLink() {
    using namespace ax;
    auto showLabel = []( const char* label, ImColor color ) {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() - ImGui::GetTextLineHeight() );
        auto size = ImGui::CalcTextSize( label );

        auto padding = ImGui::GetStyle().FramePadding;
        auto spacing = ImGui::GetStyle().ItemSpacing;

        auto cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos( ImVec2( cursorPos.x + spacing.x, cursorPos.y - spacing.y ) );

        auto screenPos = ImGui::GetCursorScreenPos();
        auto rectMin = ImVec2( screenPos.x - padding.x, screenPos.y - padding.y );
        auto rectMax = ImVec2( screenPos.x + size.x + padding.x, screenPos.y + size.y + padding.y );

        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled( rectMin, rectMax, color, size.y * 0.15f );
        ImGui::TextUnformatted( label );
    };

    if ( NodeEditor::BeginCreate( ImColor( 255, 255, 255 ), 2.0f ) ) {
        m_ForcedReroutePinDirection = GraphEditorContext::PinDirection::Invalid;

        NodeEditor::PinId startPinId = 0, endPinId = 0;
        if ( NodeEditor::QueryNewLink( &startPinId, &endPinId ) ) {
            Guid64 startPinGlobalId( startPinId.Get() );
            Guid64 endPinGlobalId( endPinId.Get() );

            if ( startPinGlobalId == endPinGlobalId ) {
                showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::ConnectSamePin ),
                           ImColor( 45, 32, 32, 180 ) );
                NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                NodeEditor::EndCreate();
                return;
            }

            // check if end or start are reroute nodes
            const RerouteNode* startRerouteNode = GetRerouteNodeByPinId( startPinGlobalId );
            bool isStartPinReroute = startRerouteNode != nullptr;

            const RerouteNode* endRerouteNode = GetRerouteNodeByPinId( endPinGlobalId );
            bool isEndPinReroute = endRerouteNode != nullptr;

            if ( isStartPinReroute ) {
                startPinGlobalId = ( startRerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input )
                                       ? startRerouteNode->InputPinId
                                       : startRerouteNode->OutputPinId;
            }

            if ( isEndPinReroute ) {
                endPinGlobalId = ( endRerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input )
                                     ? endRerouteNode->InputPinId
                                     : endRerouteNode->OutputPinId;
            }

            if ( isStartPinReroute && isEndPinReroute && ( startRerouteNode->Id == endRerouteNode->Id ) ) {
                showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::ConnectSelf ),
                           ImColor( 45, 32, 32, 180 ) );
                NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                NodeEditor::EndCreate();
                return;
            }

            const GraphEditorContext::Pin* startPin = nullptr;
            if ( isStartPinReroute == false )
                startPin = &m_EditorContext->GetPin( startPinGlobalId );

            const GraphEditorContext::Pin* endPin = nullptr;
            if ( isEndPinReroute == false )
                endPin = &m_EditorContext->GetPin( endPinGlobalId );

            const node_graph::PinTypeId startPinTypeId = isStartPinReroute ? startRerouteNode->PinTypeId
                                                                           : startPin->PinTypeId;
            const node_graph::PinTypeId endPinTypeId = isEndPinReroute ? endRerouteNode->PinTypeId : endPin->PinTypeId;

            uint32_t pinTypeColor = 0xFFFFFFFF;
            if ( isStartPinReroute )
                pinTypeColor = startRerouteNode->Color;
            else if ( isEndPinReroute )
                pinTypeColor = endRerouteNode->Color;
            else if ( startPin != nullptr )
                pinTypeColor = startPin->Color;
            else if ( endPin != nullptr )
                pinTypeColor = endPin->Color;

            if ( m_EditorContext->ArePinTypesCompatible( startPinTypeId, endPinTypeId ) == false ) {
                showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::IncompatiblePinType ),
                           ImColor( 45, 32, 32, 180 ) );
                NodeEditor::RejectNewItem( ImColor( 255, 128, 128 ), 1.0f );
                NodeEditor::EndCreate();
                return;
            }

            bool isStartPinInput = isStartPinReroute
                                       ? ( startRerouteNode->InputPinId == startPinGlobalId )
                                       : ( startPin->Direction == GraphEditorContext::PinDirection::Input );
            bool isEndPinInput = isEndPinReroute ? ( endRerouteNode->InputPinId == endPinGlobalId )
                                                 : ( endPin->Direction == GraphEditorContext::PinDirection::Input );

            if ( isStartPinInput == isEndPinInput ) {
                if ( isStartPinReroute && ( isEndPinReroute == false ) ) {
                    m_ForcedReroutePinDirection = isStartPinInput ? GraphEditorContext::PinDirection::Output
                                                                  : GraphEditorContext::PinDirection::Input;
                    isStartPinInput = !isStartPinInput;
                } else if ( isEndPinReroute && ( isStartPinReroute == false ) ) {
                    m_ForcedReroutePinDirection = isEndPinInput ? GraphEditorContext::PinDirection::Output
                                                                : GraphEditorContext::PinDirection::Input;
                    isEndPinInput = !isEndPinInput;
                }
            }

            if ( isStartPinInput && isEndPinInput ) {
                showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::TwoInputPins ),
                           ImColor( 45, 32, 32, 180 ) );
                NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                NodeEditor::EndCreate();
                return;
            }

            if ( ( isStartPinInput == false ) && ( isEndPinInput == false ) ) {
                showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::TwoOutputPins ),
                           ImColor( 45, 32, 32, 180 ) );
                NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                NodeEditor::EndCreate();
                return;
            }

            const bool isExecutePinType = ( startPinTypeId == node_graph::PinTypeId::Execute );
            if ( ( ( isStartPinInput == false ) && ( isExecutePinType == false ) ) ||
                 ( isStartPinInput && isExecutePinType ) ) {
                std::swap( startPinId, endPinId );
                std::swap( startPinGlobalId, endPinGlobalId );
                std::swap( isStartPinReroute, isEndPinReroute );
                std::swap( startRerouteNode, endRerouteNode );
            }

            DynamicArray< Guid64 > startPinIds;
            DynamicArray< Guid64 > endPinsIds;

            if ( isStartPinReroute ) {
                FindRerouteDestinations( startPinGlobalId, startPinIds );
            } else {
                startPinIds.push_back( startPinGlobalId );
            }

            if ( isEndPinReroute ) {
                FindRerouteDestinations( endPinGlobalId, endPinsIds );

            } else {
                endPinsIds.push_back( endPinGlobalId );
            }

            for ( Guid64 globalStartPinId : startPinIds ) {
                for ( Guid64 globalEndPinId : endPinsIds ) {
                    // TODO: Get out error string from this function
                    if ( m_EditorContext->IsNewLinkValid( globalStartPinId, globalEndPinId ) == false ) {
                        showLabel( format::format( "x {}", localization::editor::NodeEditor::Error::DependencyCycle ),
                                   ImColor( 45, 32, 32, 180 ) );
                        NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 1.0f );
                        NodeEditor::EndCreate();
                        return;
                    }
                }
            }

            const bool isPinLinked = m_EditorContext->IsPinLinked( startPinGlobalId );
            if ( isPinLinked ) {
                showLabel( format::format( "+ {}", localization::editor::NodeEditor::ReplaceLink ),
                           ImColor( 32, 45, 32, 180 ) );
            } else {
                showLabel( format::format( "+ {}", localization::editor::NodeEditor::CreateLink ),
                           ImColor( 32, 45, 32, 180 ) );
            }

            if ( NodeEditor::AcceptNewItem( ImColor( 128, 255, 128 ), 4.0f ) ) {
                if ( isStartPinReroute || isEndPinReroute ) {
                    // handle reroute linking
                    for ( Guid64 globalStartPinId : startPinIds ) {
                        for ( Guid64 globalEndPinId : endPinsIds ) {
                            GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink( globalStartPinId,
                                                                                                globalEndPinId );
                            newLink.IsRerouted = true;
                        }
                    }

                    // remove replaced reroute link
                    if ( isStartPinReroute ) {
                        std::ignore = std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& rerouteLink ) {
                            return rerouteLink.FromInputPinId == startPinGlobalId;
                        } );
                    } else if ( isEndPinReroute ) {
                        pinTypeColor = endRerouteNode->Color;
                        std::ignore = std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& rerouteLink ) {
                            return ( rerouteLink.FromInputPinId == startPinGlobalId );
                        } );
                    }

                    m_RerouteLinks.emplace_back( Guid64Generator::getGuid(),
                                                 Guid64( startPinGlobalId.get() ),
                                                 Guid64( endPinGlobalId.get() ),
                                                 startPinTypeId,
                                                 pinTypeColor );
                } else {
                    m_EditorContext->CreateNewLink( startPinGlobalId, endPinGlobalId );

                    std::ignore = std::erase_if( m_RerouteLinks, [ & ]( const RerouteLink& rerouteLink ) {
                        return rerouteLink.FromInputPinId == startPinGlobalId;
                    } );
                }
            }
        }

        // draw node create
        DrawCreateNode();
    }

    NodeEditor::EndCreate();
}

void NodeGraphEditorWindow::DrawCreateNode() {
    using namespace ax;

    auto showLabel = []( const char* label, ImColor color ) {
        ImGui::SetCursorPosY( ImGui::GetCursorPosY() - ImGui::GetTextLineHeight() );
        auto size = ImGui::CalcTextSize( label );

        auto padding = ImGui::GetStyle().FramePadding;
        auto spacing = ImGui::GetStyle().ItemSpacing;

        auto cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos( ImVec2( cursorPos.x + spacing.x, cursorPos.y - spacing.y ) );

        auto screenPos = ImGui::GetCursorScreenPos();
        auto rectMin = ImVec2( screenPos.x - padding.x, screenPos.y - padding.y );
        auto rectMax = ImVec2( screenPos.x + size.x + padding.x, screenPos.y + size.y + padding.y );

        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled( rectMin, rectMax, color, size.y * 0.15f );
        ImGui::TextUnformatted( label );
    };

    NodeEditor::PinId pinId = 0;
    if ( NodeEditor::QueryNewNode( &pinId ) ) {
        StringView label = format::format( "+ {}", localization::editor::NodeEditor::CreateNode );

        Guid64 globalPinId = Guid64( pinId.Get() );
        const RerouteNode* rerouteNode = GetRerouteNodeByPinId( globalPinId );
        if ( rerouteNode ) {
            bool isInputPin = rerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input;
            if ( isInputPin && ( rerouteNode->PinTypeId != node_graph::PinTypeId::Execute ) ) {
                if ( NodeEditor::PinHadAnyLinks( rerouteNode->InputPinId.get() ) ) {
                    label = format::format( "+ {}", localization::editor::NodeEditor::ReplaceNode );
                }
            }
        }

        showLabel( label.data(), ImColor( 32, 45, 32, 180 ) );

        if ( NodeEditor::AcceptNewItem() ) {
            local_IsCreatingNode = true;
            m_CreateNodeData.PinId = globalPinId;

            // check if pin is part of a reroute node
            if ( rerouteNode ) {
                m_CreateNodeData.PinId = ( rerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input )
                                             ? rerouteNode->InputPinId
                                             : rerouteNode->OutputPinId;
                m_CreateNodeData.PinTypeId = rerouteNode->PinTypeId;
                m_CreateNodeData.Direction = rerouteNode->ActivePinDirection;
            } else {
                const GraphEditorContext::Pin& pinToConnect = m_EditorContext->GetPin( m_CreateNodeData.PinId );
                m_CreateNodeData.PinTypeId = pinToConnect.PinTypeId;
                m_CreateNodeData.Direction = pinToConnect.Direction;
            }

            FilterNodeListContextMenu( "" );

            NodeEditor::Suspend();
            ImGui::OpenPopup( "Create New Node" );
            NodeEditor::Resume();
        }
    }
}

void NodeGraphEditorWindow::DrawNodeHeader( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder ) {
    if ( node.Name.empty() || ( node.ShowNodeName == false ) ) {
        return;
    }

    builder.Header( ImVec4( 0, 0, 1, 1 ) );

    ImGui::Spring( 0 );
    ImGui::TextUnformatted( node.Name.data() );
    ImGui::Spring( 1 );
    ImGui::Dummy( ImVec2( 0, 28 ) );

    ImGui::Spring( 0 );

    builder.EndHeader();
}

void NodeGraphEditorWindow::DrawNodeInputs( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder ) {
    float32 alpha = ImGui::GetStyle().Alpha;

    for ( const GraphEditorContext::Pin& inputPin : node.Inputs ) {
        builder.Input( inputPin.Id.get() );

        DrawPinIcon( inputPin.Color,
                     inputPin.PinTypeId,
                     m_EditorContext->IsPinLinked( inputPin.Id ),
                     true,
                     ImVec2( 24, 24 ),
                     (int)( alpha * 255 ) );
        ImGui::Spring( 0 );

        if ( inputPin.PinTypeId != node_graph::PinTypeId::Execute ) {
            StringView inputName = inputPin.Name;
            if ( inputName.empty() == false ) {
                ImGui::TextUnformatted( inputName.data() );
                ImGui::Spring( 0 );
            }
        }

        builder.EndInput();
    }
}

void NodeGraphEditorWindow::DrawNodeOutputs( const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder ) {
    float32 alpha = ImGui::GetStyle().Alpha;

    for ( const GraphEditorContext::Pin& outputPin : node.Outputs ) {
        builder.Output( outputPin.Id.get() );

        StringView outputName = outputPin.Name;
        if ( outputName.empty() == false ) {
            ImGui::Spring();
            ImGui::TextUnformatted( outputName.data() );
        }

        ImGui::Spring( 0 );
        DrawPinIcon( outputPin.Color,
                     outputPin.PinTypeId,
                     m_EditorContext->IsPinLinked( outputPin.Id ),
                     true,
                     ImVec2( 24, 24 ),
                     (int)( alpha * 255 ) );
        builder.EndOutput();
    }
}
} // namespace onyx::editor
