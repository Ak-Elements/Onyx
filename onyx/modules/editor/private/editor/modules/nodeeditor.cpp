#include <editor/modules/nodeeditor.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <editor/panels/nodeeditor/styles/blueprint.h>

#include <editor/nodegraph/nodegrapheditorcontext.h>
#include <editor/panels/nodeeditor/styles/widgets.h>

#include <editor/windows/editormainwindow.h>

#include <onyx/assets/asset.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/widgets.h>
#include <onyx/nodegraph/executioncontext.h>
#include <onyx/nodegraph/nodefactory.h>
#include <onyx/filesystem/filedialog.h>

#include <onyx/ui/imguisystem.h>

#include <imgui_node_editor.h>
#include <imgui_internal.h>
#include <imgui_node_editor_internal.h>

namespace Onyx::Editor
{
    namespace
    {
        constexpr onyxF32 PIN_ICON_SIZE = 24.0f;

        void DrawPinIcon(onyxU32 color, NodeGraph::PinTypeId typeId, bool connected, bool showTriangleTip, ImVec2 hitbox, int alpha)
        {
            IconType iconType = IconType::Circle;
            ImColor imColor = color;
            imColor.Value.w = alpha / 255.0f;

            if (typeId == NodeGraph::PinTypeId::Execute)
                iconType = IconType::Flow;

            Icon(ImVec2(PIN_ICON_SIZE, PIN_ICON_SIZE), hitbox, iconType, connected, showTriangleTip,imColor, ImColor(32, 32, 32, alpha));
        };

        onyxU32 CreateHighlightColor(onyxU32 color, float brightnessFactor)
        {
            // Extract RGBA components (assuming color is in RGBA format)
            unsigned char r = (color >> IM_COL32_R_SHIFT) & 0xFF;
            unsigned char g = (color >> IM_COL32_G_SHIFT) & 0xFF;
            unsigned char b = (color >> IM_COL32_B_SHIFT) & 0xFF;
            unsigned char a = (color >> IM_COL32_A_SHIFT) & 0xFF;

            // Increase brightness
            r = static_cast<unsigned char>(std::min(r * brightnessFactor, 255.0f));
            g = static_cast<unsigned char>(std::min(g * brightnessFactor, 255.0f));
            b = static_cast<unsigned char>(std::min(b * brightnessFactor, 255.0f));

            // Recombine the color
            return IM_COL32(r, g, b, a);
        }

        bool local_IsCreatingNode = false;
    }

    NodeGraphEditorWindow::NodeGraphEditorWindow(Assets::AssetSystem& assetSystem, Input::InputActionSystem& inputActionSystem)
        : m_AssetSystem(assetSystem)
        , m_InputActionSystem(inputActionSystem)
    {

        dockspace = Ui::Dockspace::Create({
            {
                Ui::DockSplitDirection::Right, 0.2f, "###PropertiesPanel", "###CanvasPanel"
            }
        });
    }

    NodeGraphEditorWindow::~NodeGraphEditorWindow() = default;

    void NodeGraphEditorWindow::OnOpen()
    {
        ax::NodeEditor::Config config;
        // disable automatic save - We have to override SaveSettings to avoid a leak if the settings file is nullptr
        config.SettingsFile = nullptr;
        config.SaveSettings = [](const char*, size_t, ax::NodeEditor::SaveReasonFlags, void*) { return true; };

        m_Context = ax::NodeEditor::CreateEditor(&config);

        m_AssetId = Assets::AssetId::Invalid;

        m_InputActionSystem.OnInput<&NodeGraphEditorWindow::OnCopyAction>(Hash::FNV1aHash64("Copy"), this);
        m_InputActionSystem.OnInput<&NodeGraphEditorWindow::OnPasteAction>(Hash::FNV1aHash64("Paste"), this);
        m_InputActionSystem.OnInput<&NodeGraphEditorWindow::OnDeleteAction>(Hash::FNV1aHash64("Delete"), this);
    }

    void NodeGraphEditorWindow::OnClose()
    {
        ax::NodeEditor::DestroyEditor(m_Context);
        m_Context = nullptr;
        m_EditorContext.reset();

        m_RerouteNodes.clear();
        m_RerouteLinks.clear();

        m_AssetId = Assets::AssetId::Invalid;

        m_InputActionSystem.Disconnect(this);
    }

    void NodeGraphEditorWindow::OnRender(Ui::ImGuiSystem& system)
    {
        if ((m_Context == nullptr) || (m_EditorContext == nullptr))
        {
            return;
        }

        using namespace ax;

        if (IsDocked())
        {
            SetWindowFlags(ImGuiWindowFlags_NoScrollWithMouse);
        }
        else
        {
            SetWindowFlags(ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
        }

        Optional<EditorMainWindow*> mainWindowOptional = system.GetWindow<EditorMainWindow>();
        if (mainWindowOptional.has_value())
        {
            EditorMainWindow& mainWindow = *mainWindowOptional.value();
            ImGui::SetNextWindowDockID(mainWindow.GetCenterDockId(), ImGuiCond_FirstUseEver);
        }


        // combine window name with graph name as the visual name
        Begin();

        if (ImGui::IsWindowAppearing())
        {
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
        }
        RenderMenuBar();

        ImGuiID dockspaceID = ImGui::GetID("NodeEditor");
        dockspace.Render(dockspaceID);

        ImGui::Begin("Unnamed Graph###CanvasPanel");

        NodeEditor::SetCurrentEditor(m_Context);
        NodeEditor::Begin("My Editor");

        if (m_EditorContext->IsLoading() == false)
        {
            DrawCanvas();
        }

        NodeEditor::End();

        ImGui::End();

        if (m_EditorContext->IsLoading() == false)
        {
            DrawPropertiesPanel();
        }

        if (m_ShouldFocus)
        {
            m_ShouldFocus = false;
            NodeEditor::NavigateToContent(false);
        }

        NodeEditor::SetCurrentEditor(nullptr);

        End();
    }

    void NodeGraphEditorWindow::RenderMenuBar()
    {
        BeginMenuBar();

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                ax::NodeEditor::SetCurrentEditor(m_Context);
                Save();
                ax::NodeEditor::SetCurrentEditor(nullptr);
            }

            if (ImGui::MenuItem("Open"))
            {
                Load();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Show Link Directions", 0, m_ShowLinkDirections))
            {
                m_ShowLinkDirections = !m_ShowLinkDirections;
            }

            ImGui::EndMenu();
        }

        EndMenuBar();
    }

    NodeGraphEditorWindow::RerouteNode::RerouteNode(NodeGraph::PinTypeId pinTypeId, onyxU32 pinTypeColor)
    {
        Id = Guid64Generator::GetGuid();
        InputPinId = Guid64Generator::GetGuid();
        OutputPinId = Guid64Generator::GetGuid();
        InteractionPinId = Guid64Generator::GetGuid();

        PinTypeId = pinTypeId;
        Color = pinTypeColor;
    }

    void NodeGraphEditorWindow::DrawContextMenu()
    {
        using namespace ax;

        NodeEditor::Suspend();

        NodeEditor::LinkId linkId = 0;
        //if (ed::ShowNodeContextMenu(&contextNodeId))
        //    ImGui::OpenPopup("Node Context Menu");
        //else if (ed::ShowPinContextMenu(&contextPinId))
        //    ImGui::OpenPopup("Pin Context Menu");
        //else
        //NodeEditor::getlin
        if (NodeEditor::ShowLinkContextMenu(&linkId))
        {
            ImGui::OpenPopup("Link Context Menu");
        }
        else if (NodeEditor::ShowBackgroundContextMenu())
        {
            ImGui::OpenPopup("Create New Node");
        }
        NodeEditor::Resume();

        ax::NodeEditor::Suspend();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
        if (ImGui::BeginPopup("Create New Node"))
        {
            if (ImGui::BeginChild("##NodesScrollList", ImVec2(350.0f, 350.0f), ImGuiChildFlags_AlwaysUseWindowPadding))
            {
                static String s_SearchString;
                static bool s_HasFocus = false;

                s_HasFocus |= ImGui::GetCurrentWindow()->Appearing;

                if (ImGui::GetCurrentWindow()->Appearing)
                    s_SearchString.clear();

                bool hasChanged = Ui::DrawSearchBar(s_SearchString, "Search nodes...", s_HasFocus);
                if (hasChanged)
                {
                    if (s_SearchString.empty() && (m_CreateNodeData.PinId.IsValid() == false))
                        m_EditorContext->ClearNodeListFilter();
                    else
                        FilterNodeListContextMenu(s_SearchString);
                }

                const GraphEditorContext::NodeListContextMenuItem& nodeListMenuRoot = m_EditorContext->GetNodeListContextMenuRoot();

                // TODO: Currently is we have a pin filter we force open all the time - we should fix that to only force open once
                bool forceOpenCollapsingHeaders = (s_SearchString.empty() == false) || m_CreateNodeData.PinId.IsValid();
                for (const GraphEditorContext::NodeListContextMenuItem& childMenu : (nodeListMenuRoot.m_Children | std::views::values))
                {
                    if (DrawCanvasContextMenuNode(childMenu, forceOpenCollapsingHeaders))
                    {
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }
            }
            ImGui::EndChild();

            ImGui::EndPopup();
        }
        else
        {
            m_CreateNodeData.PinId.Reset();
            m_CreateNodeData.PinTypeId = NodeGraph::PinTypeId::Invalid;

            local_IsCreatingNode = false;
        }

        ImGui::PopStyleVar();
        
        NodeEditor::Resume();
    }

    bool NodeGraphEditorWindow::DrawCanvasContextMenuNode(const GraphEditorContext::NodeListContextMenuItem& node, bool shouldForceOpen)
    {
        using namespace ax;

        if (node.TypeId != 0)
        {
            ONYX_ASSERT(node.m_Children.empty());

            if (ImGui::MenuItem(node.Label.c_str(), nullptr, false, true))
            {
                CreateNewNode(node);
                return true;
            }
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(140, 140, 140, 255));

            ImGui::SetNextItemOpen(false, ImGuiCond_Appearing);

            if (shouldForceOpen)
                ImGui::SetNextItemOpen(true);

            Ui::ScopedImGuiId id(node.Label);
            if (Ui::ContextMenuHeader(node.Label.c_str(), 0))
            {
                ImGui::PopStyleColor();

                ImGui::Indent();

                for (const GraphEditorContext::NodeListContextMenuItem& childNode : (node.m_Children | std::views::values))
                {
                    if (DrawCanvasContextMenuNode(childNode, shouldForceOpen))
                        return true;
                }

                ImGui::Spacing();
                ImGui::Unindent();
            }
            else
            {
                ImGui::PopStyleColor();
            }
        }

        return false;
    }

    void NodeGraphEditorWindow::OnLinkDoubleClicked(Guid64 linkId)
    {
        Guid64 fromPinId;
        Guid64 toPinId;
        
        NodeGraph::PinTypeId pinTypeId = NodeGraph::PinTypeId::Invalid;
        onyxU32 pinTypeColor = 0;

        auto rerouteLinkIt = std::ranges::find_if(m_RerouteLinks, [&](RerouteLink& link)
        {
            return link.Id == linkId;
        });

        if (rerouteLinkIt == m_RerouteLinks.end())
        {
            auto it = std::ranges::find_if(m_EditorContext->GetLinks(), [&](GraphEditorContext::Link& link)
            {
                return link.Id == linkId;
            });

            if (it != m_EditorContext->GetLinks().end())
            {
                fromPinId = it->FromPinId;
                toPinId = it->ToPinId;
                pinTypeColor = it->Color;
                pinTypeId = m_EditorContext->GetPin(it->FromPinId).PinTypeId;

                it->IsRerouted = true;
            }
        }
        else
        {
            fromPinId = rerouteLinkIt->FromInputPinId;
            toPinId = rerouteLinkIt->ToOutputPinId;
     
            pinTypeId = rerouteLinkIt->PinTypeId;
            pinTypeColor = rerouteLinkIt->Color;

            m_RerouteLinks.erase(rerouteLinkIt);
        }

        if (fromPinId.IsValid() && toPinId.IsValid())
        {
            const ImVec2 newNodePostion = ImGui::GetMousePos();
            RerouteNode& newRerouteNode = m_RerouteNodes.emplace_back(pinTypeId, pinTypeColor);
            ax::NodeEditor::SetNodePosition(newRerouteNode.Id.Get(), newNodePostion);

            m_RerouteLinks.emplace_back(Guid64Generator::GetGuid(), newRerouteNode.InputPinId, toPinId , pinTypeId, pinTypeColor);
            m_RerouteLinks.emplace_back(Guid64Generator::GetGuid(), fromPinId, newRerouteNode.OutputPinId, pinTypeId, pinTypeColor);
        }
    }

    const NodeGraphEditorWindow::RerouteNode* NodeGraphEditorWindow::GetRerouteNodeById(Guid64 nodeId) const
    {
        for (const RerouteNode& node : m_RerouteNodes)
        {
            if (node.Id == nodeId)
                return &node;
        }

        return nullptr;
    }

    const NodeGraphEditorWindow::RerouteNode* NodeGraphEditorWindow::GetRerouteNodeByPinId(Guid64 pinId) const
    {
        for (const RerouteNode& node : m_RerouteNodes)
        {
            if (node.InputPinId == pinId)
                return &node;

            if (node.OutputPinId == pinId)
                return &node;

            if (node.InteractionPinId == pinId)
                return &node;
        }

        return nullptr;
    }

    void NodeGraphEditorWindow::FindRerouteDestinations(Guid64 reroutePinId, DynamicArray<Guid64>& outDestinationPinIds)
    {
        const RerouteNode* rerouteNode = GetRerouteNodeByPinId(reroutePinId);
        if (rerouteNode == nullptr)
        {
            outDestinationPinIds.push_back(reroutePinId);
            return;
        }

        bool isWalkingBackwards = (rerouteNode->InputPinId != reroutePinId);

        Stack<Guid64> traversalStack;
        traversalStack.push(isWalkingBackwards ? rerouteNode->InputPinId : rerouteNode->OutputPinId);

        while (traversalStack.empty() == false)
        {
            Guid64 currentPinId = traversalStack.top();
            traversalStack.pop();

            for (const RerouteLink& link : m_RerouteLinks)
            {
                Guid64 linkedPinId;
                if (link.ToOutputPinId == currentPinId)
                    linkedPinId = link.FromInputPinId;
                else if (link.FromInputPinId == currentPinId)
                    linkedPinId = link.ToOutputPinId;
                else
                    continue;

                rerouteNode = GetRerouteNodeByPinId(linkedPinId);
                // if we failed to get a reroute node for the pin id, this pin has to be a real node pin 
                if (rerouteNode == nullptr)
                    outDestinationPinIds.push_back(linkedPinId);
                else
                    traversalStack.push(isWalkingBackwards ? rerouteNode->InputPinId : rerouteNode->OutputPinId);
            }
        }
    }

    void NodeGraphEditorWindow::DrawRerouteNode(RerouteNode& node)
    {
        constexpr onyxF32 PIN_ICON_SIZE_HALF = PIN_ICON_SIZE / 2.0f;
        constexpr onyxF32 PIN_ICON_SIZE_QUATER = PIN_ICON_SIZE / 4.0f;

        constexpr ImVec2 PIN_ICON_SIZE_HALF_2D(PIN_ICON_SIZE_HALF, PIN_ICON_SIZE_HALF);
        constexpr ImVec2 INTERACTION_PIN_OFFSET(PIN_ICON_SIZE_QUATER, PIN_ICON_SIZE_QUATER);
        constexpr ImVec2 HIDDEN_PIN_SIZE(0.1f, 0.1f);

        ImVec2 mousePos = ImGui::GetMousePos();
        
        Guid64 inputPinId = node.InputPinId;
        Guid64 outputPinId = node.OutputPinId;

        ax::NodeEditor::PinId hoveredPinId = ax::NodeEditor::GetHoveredPin();
        bool isPinHovered = (hoveredPinId.Get() == node.InteractionPinId.Get());
        
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodePadding, ImVec4 (10, 4, 10, 4));
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeBorderWidth, 0.0f);
        ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImVec4(0, 0, 0, 0));
        if (isPinHovered)
        {
            ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_HovNodeBorder, ImVec4(0, 0, 0, 0));
        }

        ax::NodeEditor::BeginNode(node.Id.Get());

        {
            // Input pin
            // Overlapping Input and Output Pins
            ImVec2 pinPosition = ImGui::GetCursorPos(); // Adjust as needed
            ImVec2 hiddenPinMinPosition = pinPosition + PIN_ICON_SIZE_HALF_2D - HIDDEN_PIN_SIZE;
            ImVec2 hiddenPinMaxPosition = pinPosition + PIN_ICON_SIZE_HALF_2D + HIDDEN_PIN_SIZE;

            ImGui::SetNextItemAllowOverlap();
            ImGui::Dummy(ImVec2(PIN_ICON_SIZE, PIN_ICON_SIZE));

            // Input pin
            ImGui::SetNextItemAllowOverlap();
            ImGui::SetCursorPos(pinPosition);
            ax::NodeEditor::BeginPin(inputPinId.Get(), ax::NodeEditor::PinKind::Input);
            ax::NodeEditor::PinPivotAlignment(ImVec2(0.5f, 0.5f));       // Align the pivot to the center of the area
            ax::NodeEditor::PinRect(hiddenPinMinPosition, hiddenPinMaxPosition);
            ax::NodeEditor::EndPin();

            // Output pin (shares the same pivot rect as the input pin)
            ImGui::SetNextItemAllowOverlap();

            ImGui::SetCursorPos(pinPosition);
            ax::NodeEditor::BeginPin(outputPinId.Get(), ax::NodeEditor::PinKind::Output);
            ax::NodeEditor::PinPivotAlignment(ImVec2(0.5f, 0.5f));       // Align the pivot to the center of the area
            ax::NodeEditor::PinRect(hiddenPinMinPosition, hiddenPinMaxPosition);
            ax::NodeEditor::EndPin();
            
            ax::NodeEditor::PinKind pinKind = ax::NodeEditor::PinKind::Input;
            node.ActivePinDirection = GraphEditorContext::PinDirection::Input;

            if (m_ForcedReroutePinDirection != GraphEditorContext::PinDirection::Invalid)
            {
                node.ActivePinDirection = m_ForcedReroutePinDirection;
                pinKind = m_ForcedReroutePinDirection == GraphEditorContext::PinDirection::Input ? ax::NodeEditor::PinKind::Input : ax::NodeEditor::PinKind::Output;
            }
            else
            {
                if (mousePos.x >= (pinPosition.x + PIN_ICON_SIZE_HALF_2D.x))
                {
                    pinKind = ax::NodeEditor::PinKind::Output;
                    node.ActivePinDirection = GraphEditorContext::PinDirection::Output;
                }
            }
            
            ImGui::SetCursorPos(pinPosition);

            ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRect, ImVec4(0, 0, 0, 0));
            ax::NodeEditor::BeginPin(node.InteractionPinId.Get(), pinKind);
            ax::NodeEditor::PinPivotAlignment(ImVec2(0.5f, 0.5f));       // Align the pivot to the center of the area
            ax::NodeEditor::PinRect(pinPosition + PIN_ICON_SIZE_HALF_2D - INTERACTION_PIN_OFFSET, pinPosition + PIN_ICON_SIZE_HALF_2D + INTERACTION_PIN_OFFSET);

            onyxU32 pinColor = isPinHovered ? CreateHighlightColor(node.Color, 1.5f) : node.Color;

            DrawPinIcon(pinColor, node.PinTypeId, true, false, PIN_ICON_SIZE_HALF_2D, 255);

            ax::NodeEditor::EndPin();
            ax::NodeEditor::PopStyleColor();
        }

        ax::NodeEditor::EndNode();
        ax::NodeEditor::PopStyleVar(2);
        ax::NodeEditor::PopStyleColor(isPinHovered ? 2 : 1);
    }

    void NodeGraphEditorWindow::DrawCanvas()
    {
        if (m_EditorContext == nullptr)
            return;

        const ImVec2& cursorTopLeft = ImGui::GetCursorScreenPos();

        // Start drawing nodes.
        BlueprintNodeBuilder builder;
        DynamicArray<GraphEditorContext::Node>& nodes = m_EditorContext->GetNodes();
        for (GraphEditorContext::Node& node : nodes)
        {
            if (node.HasUpdatedPosition)
            {
                ax::NodeEditor::SetNodePosition(node.Id.Get(), ImVec2(node.Position[0], node.Position[1]));
                node.HasUpdatedPosition = false;
            }

            DrawNode(node, builder);
        }

        for (RerouteNode& node : m_RerouteNodes)
        {
            if (node.HasUpdatedPosition)
            {
                ax::NodeEditor::SetNodePosition(node.Id.Get(), ImVec2(node.Position[0], node.Position[1]));
                node.HasUpdatedPosition = false;
            }

            DrawRerouteNode(node);
        }

        // draw links
        DrawNodeLinks();

        if (local_IsCreatingNode == false)
        {
            // draw link create
            DrawCreateLink();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);

        DrawContextMenu();

        if (ax::NodeEditor::LinkId linkId = ax::NodeEditor::GetDoubleClickedLink())
        {
            OnLinkDoubleClicked(Guid64(linkId.Get()));
        }
    }

    void NodeGraphEditorWindow::DrawPropertiesPanel()
    {
        using namespace ax;

        {
            Ui::ScopedImGuiStyle dockspaceStyle
            {
                { ImGuiStyleVar_FrameBorderSize, 0.0f },
                { ImGuiStyleVar_ItemSpacing, ImVec2(0.0, 0.0f) },
                { ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0, 0.0f) }
            };

            ImGui::Begin("Properties###PropertiesPanel", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        }

        DynamicArray<NodeEditor::NodeId> selectedNodes;
        selectedNodes.resize(NodeEditor::GetSelectedObjectCount());
        int nodeCount = NodeEditor::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        selectedNodes.resize(nodeCount);

        // TODO: Why is vertical group not working with property grid?
        
        ImGui::BeginVertical("##SelectedNodeProperties");
        
        // The property grid should allow editing the same property of multiple nodes at the same time if multiselected
        // e.g.: if 2 Add Floats are selected, you can edit the inputs of both with 1 input field
        // For that we have to collect all nodes with the same PinID

        for (NodeEditor::NodeId nodeId : selectedNodes)
        {
            Guid64 globalNodeId(nodeId.Get());

            if (GetRerouteNodeById(globalNodeId) != nullptr)
                continue;

            ImGui::PushID(nodeId.AsPointer<uintptr_t>());

            {
                Ui::ScopedImGuiStyle nodePanelStyle
                {
                    { ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f) }
                };
                ImGui::BeginChild("##Node", ImVec2(0, 0), ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_FrameStyle);
            }

            {
                Ui::ScopedImGuiStyle propGridStyle
                {
                    { ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f) }
                };

                Ui::PropertyGrid::BeginPropertyGrid("##propertyGrid", 120.0f);
                m_EditorContext->DrawNodeInPropertyPanel(globalNodeId);
                Ui::PropertyGrid::EndPropertyGrid();
            }

            ImGui::EndChild();

            ImGui::PopID();
        }

        ImGui::EndVertical();

        ImGui::End();
    }

    void NodeGraphEditorWindow::FilterNodeListContextMenu(const StringView& searchString)
    {
        m_EditorContext->FilterNodeListContextMenu([&](const NodeGraph::NodeEditorMetaData& nodeMetaData)
            {
                if (IgnoreCaseFind(nodeMetaData.FullyQualifiedName, searchString) == StringView::npos)
                    return false;

                if (m_CreateNodeData.PinTypeId != NodeGraph::PinTypeId::Invalid)
                {
                    if (m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Input)
                    {
                        for (const NodeGraph::PinTypeId& outputType : nodeMetaData.OutputPins)
                        {
                            if (outputType == m_CreateNodeData.PinTypeId)
                                return true;
                        }

                        return false;
                    }
                    else if (m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Output)
                    {
                        for (const NodeGraph::PinTypeId& inputType : nodeMetaData.InputPins)
                        {
                            if (inputType == m_CreateNodeData.PinTypeId)
                                return true;
                        }

                        return false;
                    }
                }

                return true;
            }
        );
    }

    void NodeGraphEditorWindow::CreateNewNode(const GraphEditorContext::NodeListContextMenuItem& node)
    {
        const ImVec2 newNodePostion = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePosOnOpeningCurrentPopup());

        GraphEditorContext::Node& newNode = m_EditorContext->CreateNewNode(node.TypeId);
        ax::NodeEditor::SetNodePosition(newNode.Id.Get(), newNodePostion);

        if (m_CreateNodeData.PinId)
        {
            // check if pin is a reroute pin and get original pin for the real graph connection
            Guid64 fromPin = m_CreateNodeData.PinId;
            Guid64 toPin = m_CreateNodeData.PinId;
            if (m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Input)
            {
                for (const GraphEditorContext::Pin& outputPin : newNode.Outputs)
                {
                    if (outputPin.PinTypeId == m_CreateNodeData.PinTypeId)
                    {
                        toPin = outputPin.Id;
                        break;
                    }
                }
            }
            else if (m_CreateNodeData.Direction == GraphEditorContext::PinDirection::Output)
            {
                for (const GraphEditorContext::Pin& inputPin : newNode.Inputs)
                {
                    if (inputPin.PinTypeId == m_CreateNodeData.PinTypeId)
                    {
                        fromPin = inputPin.Id;
                        break;
                    }
                }
            }

            if (m_CreateNodeData.PinTypeId == NodeGraph::PinTypeId::Execute)
            {
                std::swap(fromPin, toPin);
            }

            const RerouteNode* rerouteNode = GetRerouteNodeByPinId(m_CreateNodeData.PinId);
            if (rerouteNode == nullptr)
            {
                m_EditorContext->CreateNewLink(fromPin, toPin);
            }
            else
            {
                DynamicArray<Guid64> rerouteDestinationPinIds;
                FindRerouteDestinations(m_CreateNodeData.PinId, rerouteDestinationPinIds);

                // delete any existing link
                std::ignore = std::erase_if(m_RerouteLinks, [&](const RerouteLink& rerouteLink)
                {
                    return rerouteLink.FromInputPinId == m_CreateNodeData.PinId;
                });

                m_RerouteLinks.emplace_back(Guid64Generator::GetGuid(), fromPin, toPin, m_CreateNodeData.PinTypeId, rerouteNode->Color);

                for (Guid64 rerouteDestinationPinId : rerouteDestinationPinIds)
                {
                    if (fromPin == m_CreateNodeData.PinId)
                    {
                        GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink(rerouteDestinationPinId, toPin);
                        newLink.IsRerouted = true;
                    }
                    else
                    {
                        GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink(fromPin, rerouteDestinationPinId);
                        newLink.IsRerouted = true;
                    }
                }
            }
        }

        local_IsCreatingNode = false;
        m_EditorContext->ClearNodeListFilter();
    }


    void NodeGraphEditorWindow::Save()
    {
        FileSystem::Filepath path;
        if (FileSystem::FileDialog::SaveFileDialog(path, "Graph", m_EditorContext->GetExtensions()))
        {
            Assets::AssetMetaData dummyAsset;
            dummyAsset.Path = path;
            dummyAsset.Id = Assets::AssetId(dummyAsset.Path);

            if (m_EditorContext->Compile() == false)
            {
                return;
            }

            m_EditorContext->Save(m_AssetSystem, dummyAsset);
        }
    }

    void NodeGraphEditorWindow::Load()
    {
        FileSystem::Filepath path;
        if (FileSystem::FileDialog::OpenFileDialog(path, "Graph", m_EditorContext->GetExtensions()))
        {
            m_EditorContext->Load(m_AssetSystem, path);
        }
    }

    void NodeGraphEditorWindow::SaveEditorMetaData(const FileSystem::Filepath& path)
    {
        using namespace FileSystem;
        JsonValue jsonRoot;

        JsonValue nodesJsonArray;
        for (const GraphEditorContext::Node& node : m_EditorContext->GetNodes())
        {
            JsonValue nodeMetaInfo;
            nodeMetaInfo.Set("id", node.Id);
            nodeMetaInfo.Set("name", node.Name);

            ImVec2 nodePosition = ax::NodeEditor::GetNodePosition(node.Id.Get());
            std::array<onyxF32, 2> position{ nodePosition.x, nodePosition.y };
            nodeMetaInfo.Set("position", position);

            nodesJsonArray.Add(nodeMetaInfo);
        }

        jsonRoot.Set("nodes", nodesJsonArray);

        // this is added here to save the visibility state of links (for hidden links based on reroute nodes)
        JsonValue linksJsonArray;
        for (const GraphEditorContext::Link& link : m_EditorContext->GetLinks())
        {
            if (link.IsRerouted == false)
                continue;

            JsonValue linkMetaInfo;
            linkMetaInfo.Set("frominputpin", link.FromPinId);
            linkMetaInfo.Set("tooutputpin", link.ToPinId);

            linksJsonArray.Add(linkMetaInfo);
        }

        if (linksJsonArray.Json.empty() == false)
            jsonRoot.Set("hiddenlinks", linksJsonArray);

        if (m_RerouteNodes.empty() == false)
        {
            JsonValue rerouteNodesJsonArray;
            for (const RerouteNode& node : m_RerouteNodes)
            {
                JsonValue nodeMetaInfo;
                nodeMetaInfo.Set("id", node.Id);
                nodeMetaInfo.Set("inputpin", node.InputPinId);
                nodeMetaInfo.Set("outputpin", node.OutputPinId);

                ImVec2 nodePosition = ax::NodeEditor::GetNodePosition(node.Id.Get());
                std::array<onyxF32, 2> position{ nodePosition.x, nodePosition.y };
                nodeMetaInfo.Set("position", position);

                rerouteNodesJsonArray.Add(nodeMetaInfo);
            }

            jsonRoot.Set("reroutenodes", rerouteNodesJsonArray);
        }

        if (m_RerouteLinks.empty() == false)
        {
            JsonValue rerouteLinksJsonArray;
            for (const RerouteLink& link : m_RerouteLinks)
            {
                JsonValue linkMetaInfo;
                linkMetaInfo.Set("id", link.Id);
                linkMetaInfo.Set("frominputpin", link.FromInputPinId);
                linkMetaInfo.Set("tooutputpin", link.ToOutputPinId);

                rerouteLinksJsonArray.Add(linkMetaInfo);
            }

            jsonRoot.Set("reroutelinks", rerouteLinksJsonArray);
        }

        Filepath metaFilePath = Path::ReplaceExtension(path, "ometa");
        OnyxFile metaDataFile(metaFilePath);
        FileStream stream = metaDataFile.OpenStream(OpenMode::Write | OpenMode::Text);

        const String& jsonString = jsonRoot.Json.dump(4);
        stream.WriteRaw(jsonString.data(), jsonString.size());
    }

    void NodeGraphEditorWindow::LoadEditorMetaData(const FileSystem::Filepath& path)
    {
        // clear reroute nodes
        m_RerouteNodes.clear();
        m_RerouteLinks.clear();

        using namespace FileSystem;
        Filepath metaFilePath = FileSystem::Path::ReplaceExtension(path, "ometa");
        OnyxFile metaDataJsonFile(metaFilePath);

        const JsonValue& metaDataJsonRoot = metaDataJsonFile.LoadJson();

        JsonValue nodesJsonArray;
        if (metaDataJsonRoot.Get("nodes", nodesJsonArray) == false)
        {
            // fallback to root if we don't find nodes array in json, this is to allow loading of old meta data
            nodesJsonArray.Json = metaDataJsonRoot.Json;
        }

        for (const auto& nodeMetaJson : nodesJsonArray.Json)
        {
            JsonValue nodeMetaJsonObj{ nodeMetaJson };

            Guid64 nodeId;
            String nodeName;
            nodeMetaJsonObj.Get("id", nodeId);
            nodeMetaJsonObj.Get("name", nodeName);

            std::array<onyxF32, 2> position{};
            nodeMetaJsonObj.Get("position", position);

            GraphEditorContext::Node& node = m_EditorContext->GetNode(nodeId);
            node.Name = nodeName;
            node.Position = Vector2f(position[0], position[1]);
            node.HasUpdatedPosition = true;
        }

        JsonValue linksJsonArray;
        if (metaDataJsonRoot.Get("hiddenlinks", linksJsonArray))
        {
            for (const auto& linkMetaJson : linksJsonArray.Json)
            {
                JsonValue linkMetaJsonObj{ linkMetaJson };

                Guid64 fromInputPinId;
                Guid64 toOutputPinId;
                linkMetaJsonObj.Get("frominputpin", fromInputPinId);
                linkMetaJsonObj.Get("tooutputpin", toOutputPinId);

                auto it = std::ranges::find_if(m_EditorContext->GetLinks(), [&](const GraphEditorContext::Link& link)
                {
                    return (link.FromPinId == fromInputPinId) && (link.ToPinId == toOutputPinId);
                });

                if (it == m_EditorContext->GetLinks().end())
                {
                    ONYX_LOG_ERROR("Missing link from node graph");
                    continue;
                }

                it->IsRerouted = true;
            }
        }

        JsonValue rerouteNodesJsonArray;
        if (metaDataJsonRoot.Get("reroutenodes", rerouteNodesJsonArray))
        {
            for (const auto& rerouteNodeMetaJson : rerouteNodesJsonArray.Json)
            {
                JsonValue rerouteNodeMetaJsonObj{ rerouteNodeMetaJson };

                RerouteNode& newReroute = m_RerouteNodes.emplace_back();
                newReroute.InteractionPinId = Guid64Generator::GetGuid();
                rerouteNodeMetaJsonObj.Get("id", newReroute.Id);
                rerouteNodeMetaJsonObj.Get("inputpin", newReroute.InputPinId);
                rerouteNodeMetaJsonObj.Get("outputpin", newReroute.OutputPinId);

                rerouteNodeMetaJsonObj.Get("color", newReroute.Color);

                std::array<onyxF32, 2> position{};
                rerouteNodeMetaJsonObj.Get("position", position);
                newReroute.Position = Vector2f(position[0], position[1]);
                newReroute.HasUpdatedPosition = true;
            }
        }

        JsonValue rerouteLinksJsonArray;
        if (metaDataJsonRoot.Get("reroutelinks", rerouteLinksJsonArray))
        {
            for (const auto& rerouteLinkMetaJson : rerouteLinksJsonArray.Json)
            {
                JsonValue rerouteLinkMetaJsonObj{ rerouteLinkMetaJson };

                RerouteLink& newLink = m_RerouteLinks.emplace_back();
                rerouteLinkMetaJsonObj.Get("id", newLink.Id);
                rerouteLinkMetaJsonObj.Get("frominputpin", newLink.FromInputPinId);
                rerouteLinkMetaJsonObj.Get("tooutputpin", newLink.ToOutputPinId);

                //onyxU32 pinTypeId;
                //rerouteLinkMetaJsonObj.Get("pintype", pinTypeId);
                //newLink.PinTypeId = Enums::ToEnum<NodeGraph::PinTypeId>(pinTypeId);

                rerouteLinkMetaJsonObj.Get("color", newLink.Color);
            }
        }

        // TODO: Fix this to make the retrieval smarter and maybe remove typeId and color from the reroute links
        for (RerouteNode& node : m_RerouteNodes)
        {
            DynamicArray<Guid64> destinations;
            FindRerouteDestinations(node.InputPinId, destinations);

            if (destinations.empty() == false)
            {
                const GraphEditorContext::Pin& pin = m_EditorContext->GetPin(destinations.front());
                node.PinTypeId = pin.PinTypeId;
                node.Color = pin.Color;
            }
            else
            {
                FindRerouteDestinations(node.OutputPinId, destinations);
                if (destinations.empty() == false)
                {
                    const GraphEditorContext::Pin& pin = m_EditorContext->GetPin(destinations.front());
                    node.PinTypeId = pin.PinTypeId;
                    node.Color = pin.Color;
                }
            }
        }

        for (RerouteLink& link : m_RerouteLinks)
        {
            DynamicArray<Guid64> destinations;
            FindRerouteDestinations(link.FromInputPinId, destinations);

            if (destinations.empty() == false)
            {
                const GraphEditorContext::Pin& pin = m_EditorContext->GetPin(destinations.front());
                link.PinTypeId = pin.PinTypeId;
                link.Color = pin.Color;
            }
            else
            {
                FindRerouteDestinations(link.ToOutputPinId, destinations);
                if (destinations.empty() == false)
                {
                    const GraphEditorContext::Pin& pin = m_EditorContext->GetPin(destinations.front());
                    link.PinTypeId = pin.PinTypeId;
                    link.Color = pin.Color;
                }
            }
        }
    }

    void NodeGraphEditorWindow::OnGraphLoaded()
    {
        m_ShouldFocus = true;
        m_FocusDuration = 0.0f;
    }

    void NodeGraphEditorWindow::OnGraphSaved()
    {
        
    }

    void NodeGraphEditorWindow::OnCopyAction(const Input::InputActionEvent& inputActionContext)
    {
        ONYX_UNUSED(inputActionContext);
    }

    void NodeGraphEditorWindow::OnPasteAction(const Input::InputActionEvent& inputActionContext)
    {
        ONYX_UNUSED(inputActionContext);
    }

    void NodeGraphEditorWindow::OnDeleteAction(const Input::InputActionEvent& /*inputActionContext*/)
    {
        ax::NodeEditor::SetCurrentEditor(m_Context);
        DynamicArray<ax::NodeEditor::NodeId> selectedNodes;
        selectedNodes.resize(ax::NodeEditor::GetSelectedObjectCount());
        int nodeCount = ax::NodeEditor::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
        selectedNodes.resize(nodeCount);

        for (ax::NodeEditor::NodeId selectedNode : selectedNodes)
        {
            Guid64 globalNodeId(selectedNode.Get());
            if (const RerouteNode* rerouteNode = GetRerouteNodeById(globalNodeId))
            {
                for (RerouteLink& inputLink : m_RerouteLinks)
                {
                    if (inputLink.FromInputPinId != rerouteNode->InputPinId)
                        continue;

                    for (RerouteLink& outputLink : m_RerouteLinks)
                    {
                        if (outputLink.ToOutputPinId != rerouteNode->OutputPinId)
                            continue;

                        outputLink.ToOutputPinId = inputLink.ToOutputPinId;
                    }
                }

                std::erase_if(m_RerouteLinks, [&](const RerouteLink& link)
                {
                    return (link.FromInputPinId == rerouteNode->InputPinId) || (link.ToOutputPinId == rerouteNode->OutputPinId);
                });

                std::erase_if(m_RerouteNodes, [&](const RerouteNode& node)
                {
                    return node.Id == rerouteNode->Id;  
                });
            }
            else
            {
                //TODO: A remove link callback might be smarter than brute forcing this here
                const GraphEditorContext::Node& node = m_EditorContext->GetNode(globalNodeId);
                for (const GraphEditorContext::Pin& inputPin : node.Inputs)
                {
                    std::erase_if(m_RerouteLinks, [&](const RerouteLink& link)
                    {
                        return (link.FromInputPinId == inputPin.Id) || (link.ToOutputPinId == inputPin.Id);
                    });
                }

                for (const GraphEditorContext::Pin& outputPin : node.Outputs)
                {
                    std::erase_if(m_RerouteLinks, [&](const RerouteLink& link)
                    {
                        return (link.FromInputPinId == outputPin.Id) || (link.ToOutputPinId == outputPin.Id);
                    });
                }
            }

            m_EditorContext->DeleteNode(globalNodeId);
            ax::NodeEditor::DeleteNode(selectedNode);
        }

        ax::NodeEditor::ClearSelection();
        ax::NodeEditor::SetCurrentEditor(nullptr);
    }

    void NodeGraphEditorWindow::DrawNode(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder)
    {
        builder.Begin(node.Id.Get());

        DrawNodeHeader(node, builder);

        DrawNodeInputs(node, builder);
        m_EditorContext->DrawNode(node);

        DrawNodeOutputs(node, builder);

        builder.End();
    }

    void NodeGraphEditorWindow::DrawNodeLinks() const
    {
        for (const RerouteLink& reroutedLink : m_RerouteLinks)
        {
            const onyxU64 linkId = reroutedLink.Id.Get();
            ax::NodeEditor::Link(linkId, reroutedLink.FromInputPinId.Get(), reroutedLink.ToOutputPinId.Get(), ImGui::ColorConvertU32ToFloat4(reroutedLink.Color), 4);

            if (m_ShowLinkDirections)
                ax::NodeEditor::Flow(linkId, ax::NodeEditor::FlowDirection::Backward);
        }

       for (const GraphEditorContext::Link& link : m_EditorContext->GetLinks())
       {
           if (link.IsRerouted)
               continue;

           const onyxU64 linkId = link.Id.Get();
           ax::NodeEditor::Link(linkId, link.FromPinId.Get(), link.ToPinId.Get(), ImGui::ColorConvertU32ToFloat4(link.Color), 4);

           if (m_ShowLinkDirections)
               ax::NodeEditor::Flow(linkId, ax::NodeEditor::FlowDirection::Backward);
       }
    }

   void NodeGraphEditorWindow::DrawCreateLink()
   {
       using namespace ax;
       auto showLabel = [](const char* label, ImColor color)
       {
           ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
           auto size = ImGui::CalcTextSize(label);

           auto padding = ImGui::GetStyle().FramePadding;
           auto spacing = ImGui::GetStyle().ItemSpacing;

           auto cursorPos = ImGui::GetCursorPos();
           ImGui::SetCursorPos(ImVec2(cursorPos.x + spacing.x, cursorPos.y - spacing.y));

           auto screenPos = ImGui::GetCursorScreenPos();
           auto rectMin = ImVec2(screenPos.x - padding.x, screenPos.y - padding.y);
           auto rectMax = ImVec2(screenPos.x + size.x + padding.x, screenPos.y + size.y + padding.y);

           auto drawList = ImGui::GetWindowDrawList();
           drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
           ImGui::TextUnformatted(label);
       };

       if (NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
       {
           m_ForcedReroutePinDirection = GraphEditorContext::PinDirection::Invalid;

           NodeEditor::PinId startPinId = 0, endPinId = 0;
           if (NodeEditor::QueryNewLink(&startPinId, &endPinId))
           {
               Guid64 startPinGlobalId(startPinId.Get());
               Guid64 endPinGlobalId(endPinId.Get());

               if (startPinGlobalId == endPinGlobalId)
               {
                   showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                   NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                   NodeEditor::EndCreate();
                   return;
               }

               // check if end or start are reroute nodes
               const RerouteNode* startRerouteNode = GetRerouteNodeByPinId(startPinGlobalId);
               bool isStartPinReroute = startRerouteNode != nullptr;

               const RerouteNode* endRerouteNode = GetRerouteNodeByPinId(endPinGlobalId);
               bool isEndPinReroute = endRerouteNode != nullptr;

               if (isStartPinReroute)
               {
                   startPinGlobalId = (startRerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input) ? startRerouteNode->InputPinId : startRerouteNode->OutputPinId;
               }
               
               if (isEndPinReroute)
               {
                   endPinGlobalId = (endRerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input) ? endRerouteNode->InputPinId : endRerouteNode->OutputPinId;
               }

               if (isStartPinReroute && isEndPinReroute && (startRerouteNode->Id == endRerouteNode->Id))
               {
                   showLabel("x Can't connect pins from same node", ImColor(45, 32, 32, 180));
                   NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                   NodeEditor::EndCreate();
                   return;
               }

               const GraphEditorContext::Pin* startPin = nullptr;
               if (isStartPinReroute == false)
                   startPin = &m_EditorContext->GetPin(startPinGlobalId);

               const GraphEditorContext::Pin* endPin = nullptr;
               if (isEndPinReroute == false)
                   endPin = &m_EditorContext->GetPin(endPinGlobalId);

               const NodeGraph::PinTypeId startPinTypeId = isStartPinReroute ? startRerouteNode->PinTypeId : startPin->PinTypeId;
               const NodeGraph::PinTypeId endPinTypeId = isEndPinReroute ? endRerouteNode->PinTypeId : endPin->PinTypeId;

               onyxU32 pinTypeColor = 0xFFFFFFFF;
               if (isStartPinReroute)
                   pinTypeColor = startRerouteNode->Color;
               else if (isEndPinReroute)
                   pinTypeColor = endRerouteNode->Color;
               else if (startPin != nullptr)
                   pinTypeColor = startPin->Color;
               else if (endPin != nullptr)
                   pinTypeColor = endPin->Color;

               if (m_EditorContext->ArePinTypesCompatible(startPinTypeId, endPinTypeId) == false)
               {
                   showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                   NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                   NodeEditor::EndCreate();
                   return;
               }

               bool isStartPinInput = isStartPinReroute ? (startRerouteNode->InputPinId == startPinGlobalId): (startPin->Direction == GraphEditorContext::PinDirection::Input);
               bool isEndPinInput = isEndPinReroute ? (endRerouteNode->InputPinId == endPinGlobalId) : (endPin->Direction == GraphEditorContext::PinDirection::Input);
               
               if (isStartPinInput == isEndPinInput)
               {
                   if (isStartPinReroute && (isEndPinReroute == false))
                   {
                       m_ForcedReroutePinDirection = isStartPinInput ? GraphEditorContext::PinDirection::Output : GraphEditorContext::PinDirection::Input;
                       isStartPinInput = !isStartPinInput;
                   }
                   else if (isEndPinReroute && (isStartPinReroute == false))
                   {
                       m_ForcedReroutePinDirection = isEndPinInput ? GraphEditorContext::PinDirection::Output : GraphEditorContext::PinDirection::Input;
                       isEndPinInput = !isEndPinInput;
                   }
               }

               if (isStartPinInput && isEndPinInput)
               {
                   showLabel("x Can't connect two inputs", ImColor(45, 32, 32, 180));
                   NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                   NodeEditor::EndCreate();
                   return;
               }

               if ((isStartPinInput == false) && (isEndPinInput == false))
               {
                   showLabel("x Can't connect two outputs", ImColor(45, 32, 32, 180));
                   NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                   NodeEditor::EndCreate();
                   return;
               }

               const bool isExecutePinType = (startPinTypeId == NodeGraph::PinTypeId::Execute);
               if (((isStartPinInput == false) && (isExecutePinType == false)) ||
                   (isStartPinInput && isExecutePinType))
               {
                   std::swap(startPinId, endPinId);
                   std::swap(startPinGlobalId, endPinGlobalId);
                   std::swap(isStartPinReroute, isEndPinReroute);
                   std::swap(startRerouteNode, endRerouteNode);
               }


               DynamicArray<Guid64> startPinIds;
               DynamicArray<Guid64> endPinsIds;

               if (isStartPinReroute)
               {
                   FindRerouteDestinations(startPinGlobalId, startPinIds);
               }
               else
               {
                   startPinIds.push_back(startPinGlobalId);
               }

               if (isEndPinReroute)
               {
                   FindRerouteDestinations(endPinGlobalId, endPinsIds);

               }
               else
               {
                   endPinsIds.push_back(endPinGlobalId);
               }

               for (Guid64 globalStartPinId : startPinIds)
               {
                   for (Guid64 globalEndPinId : endPinsIds)
                   {
                       // TODO: Get out error string from this function
                       if (m_EditorContext->IsNewLinkValid(globalStartPinId, globalEndPinId) == false)
                       {
                           showLabel("x Cannot connect as it would create a dependency cycle", ImColor(45, 32, 32, 180));
                           NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                           NodeEditor::EndCreate();
                           return;
                       }
                   }
               }

               const bool isPinLinked = m_EditorContext->IsPinLinked(startPinGlobalId);
               if (isPinLinked)
               {
                   showLabel("+ Replace Link", ImColor(32, 45, 32, 180));
               }
               else
               {
                   showLabel("+ Create Link", ImColor(32, 45, 32, 180));
               }

               if (NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
               {
                   if (isStartPinReroute || isEndPinReroute)
                   {
                       // handle reroute linking
                       for (Guid64 globalStartPinId : startPinIds)
                       {
                           for (Guid64 globalEndPinId : endPinsIds)
                           {
                               GraphEditorContext::Link& newLink = m_EditorContext->CreateNewLink(globalStartPinId, globalEndPinId);
                               newLink.IsRerouted = true;
                           }
                       }

                       // remove replaced reroute link
                       if (isStartPinReroute)
                       {
                           std::ignore = std::erase_if(m_RerouteLinks, [&](const RerouteLink& rerouteLink)
                           {
                               return rerouteLink.FromInputPinId == startPinGlobalId;
                           });
                       }
                       else if (isEndPinReroute)
                       {
                           pinTypeColor = endRerouteNode->Color;
                           std::ignore = std::erase_if(m_RerouteLinks, [&](const RerouteLink& rerouteLink)
                           {
                               return (rerouteLink.FromInputPinId == startPinGlobalId);
                           });
                       }

                       m_RerouteLinks.emplace_back(Guid64Generator::GetGuid(), Guid64(startPinGlobalId.Get()), Guid64(endPinGlobalId.Get()), startPinTypeId, pinTypeColor);
                   }
                   else
                   {
                       m_EditorContext->CreateNewLink(startPinGlobalId, endPinGlobalId);

                       std::ignore = std::erase_if(m_RerouteLinks, [&](const RerouteLink& rerouteLink)
                       {
                            return rerouteLink.FromInputPinId == startPinGlobalId;
                       });
                   }
               }
           }

           // draw node create
           DrawCreateNode();
       }

       NodeEditor::EndCreate();
   }

   void NodeGraphEditorWindow::DrawCreateNode()
   {
       using namespace ax;

       auto showLabel = [](const char* label, ImColor color)
           {
               ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
               auto size = ImGui::CalcTextSize(label);

               auto padding = ImGui::GetStyle().FramePadding;
               auto spacing = ImGui::GetStyle().ItemSpacing;

               auto cursorPos = ImGui::GetCursorPos();
               ImGui::SetCursorPos(ImVec2(cursorPos.x + spacing.x, cursorPos.y - spacing.y));

               auto screenPos = ImGui::GetCursorScreenPos();
               auto rectMin = ImVec2(screenPos.x - padding.x, screenPos.y - padding.y);
               auto rectMax = ImVec2(screenPos.x + size.x + padding.x, screenPos.y + size.y + padding.y);

               auto drawList = ImGui::GetWindowDrawList();
               drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
               ImGui::TextUnformatted(label);
           };

       NodeEditor::PinId pinId = 0;
       if (NodeEditor::QueryNewNode(&pinId))
       {
           String label = "+ Create Node";

           Guid64 globalPinId = Guid64(pinId.Get());
           const RerouteNode* rerouteNode = GetRerouteNodeByPinId(globalPinId);
           if (rerouteNode)
           {
               bool isInputPin = rerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input;
               if (isInputPin && (rerouteNode->PinTypeId != NodeGraph::PinTypeId::Execute))
               {
                   if (NodeEditor::PinHadAnyLinks(rerouteNode->InputPinId.Get()))
                   {
                       label = "+ Replace Node";
                   }
               }
           }

           showLabel(label.data(), ImColor(32, 45, 32, 180));

           if (NodeEditor::AcceptNewItem())
           {
               local_IsCreatingNode = true;
               m_CreateNodeData.PinId = globalPinId;

               // check if pin is part of a reroute node
               if (rerouteNode)
               {
                   m_CreateNodeData.PinId = (rerouteNode->ActivePinDirection == GraphEditorContext::PinDirection::Input) ? rerouteNode->InputPinId : rerouteNode->OutputPinId;
                   m_CreateNodeData.PinTypeId = rerouteNode->PinTypeId;
                   m_CreateNodeData.Direction = rerouteNode->ActivePinDirection;
               }
               else
               {
                   const GraphEditorContext::Pin& pinToConnect = m_EditorContext->GetPin(m_CreateNodeData.PinId);
                   m_CreateNodeData.PinTypeId = pinToConnect.PinTypeId;
                   m_CreateNodeData.Direction = pinToConnect.Direction;
               }

               FilterNodeListContextMenu("");

               NodeEditor::Suspend();
               ImGui::OpenPopup("Create New Node");
               NodeEditor::Resume();
           }
       }
   }

   void NodeGraphEditorWindow::DrawNodeHeader(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder)
    {
        ONYX_UNUSED(node);

        builder.Header(ImVec4(0, 0, 1, 1));

        ImGui::Spring(0);
        ImGui::TextUnformatted(node.Name.data());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));

        ImGui::Spring(0);

        builder.EndHeader();
    }

    void NodeGraphEditorWindow::DrawNodeInputs(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder)
    {
        onyxF32 alpha = ImGui::GetStyle().Alpha;

        for (const GraphEditorContext::Pin& inputPin : node.Inputs)
        {
            builder.Input(inputPin.Id.Get());

            DrawPinIcon(inputPin.Color, inputPin.PinTypeId, m_EditorContext->IsPinLinked(inputPin.Id), true, ImVec2(24, 24), (int)(alpha * 255));
            ImGui::Spring(0);

            if (inputPin.PinTypeId != NodeGraph::PinTypeId::Execute)
            {
                const StringView& inputName = inputPin.Name;
                if (inputName.empty() == false)
                {
                    ImGui::TextUnformatted(inputName.data());
                    ImGui::Spring(0);
                }
            }

            builder.EndInput();
        }
    }

    void NodeGraphEditorWindow::DrawNodeOutputs(const GraphEditorContext::Node& node, BlueprintNodeBuilder& builder)
    {
        onyxF32 alpha = ImGui::GetStyle().Alpha;

        for (const GraphEditorContext::Pin& outputPin : node.Outputs)
        {
            builder.Output(outputPin.Id.Get());

            const StringView& outputName = outputPin.Name;
            if (outputName.empty() == false)
            {
                ImGui::Spring();
                ImGui::TextUnformatted(outputName.data());
            }

            ImGui::Spring(0);
            DrawPinIcon(outputPin.Color, outputPin.PinTypeId, m_EditorContext->IsPinLinked(outputPin.Id), true, ImVec2(24, 24), (int)(alpha * 255));
            builder.EndOutput();
        }
    }
}
