#include <onyx/ui/windows/enginevariableswindow.h>

#include <onyx/localization/localizationmodule.h>

#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/widgets.h>

namespace onyx::ui
{
    void EngineVariablesWindow::OnOpen()
    {
        RebuildVariableGraph();
    }

    void EngineVariablesWindow::OnRender(ImGuiSystem& /*imguiSystem*/)
    {
        Begin();

        bool hasSearchChanged = DrawSearchBar(m_Search, "Search", m_SearchBarIsFocused);
        if (hasSearchChanged)
        {
            RebuildVariableGraph();
        }

        property_grid::BeginPropertyGrid("Engine Vars", 80);


        RenderGraphNode(m_Root);
        property_grid::EndPropertyGrid();

        End();
    }

    void EngineVariablesWindow::RenderGraphNode(const EngineVariablesWindow::EngineVariableGraphNode& graphNode) const
    {
        const bool isSearching = m_Search.empty() == false;
        ImGuiTreeNodeFlags flags = isSearching ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;

        const bool isRoot = &graphNode == &m_Root;
        bool isOpen = true;
        if (isRoot == false)
        {
            isOpen = property_grid::BeginCollapsiblePropertyGroup(graphNode.Label, flags);
        }

        if ( isOpen )
        {
            for (const Variant<EngineVariableGraphNode, IEngineVariable*>& child : graphNode.Children)
            {
                if (const EngineVariableGraphNode* parent = std::get_if<EngineVariableGraphNode>(&child))
                {
                    RenderGraphNode(*parent);
                }
                else if (IEngineVariable* variable = std::get<IEngineVariable*>(child))
                {
                    PropertyInspectors::Draw(variable->GetRuntimeTypeId(), variable, false);
                }
            }

            if ( isRoot == false )
            {
                property_grid::EndPropertyGroup();
            }
        }
    }

    void EngineVariablesWindow::RebuildVariableGraph()
    {
        m_Root.Children.clear();

        const localization::LocalizationModule& localizationSystem = GetEngineSystem<localization::LocalizationModule>();
        HashMap<StringId32, IEngineVariable*> engineVariables = IEngine::GetVariablesRegistry().GetVariables();

        for (auto&& [id, variable] : engineVariables)
        {
            StringView localized = localizationSystem.TryGetLocalized(id).value_or(id.GetString());
            if (IgnoreCaseFind(localized, m_Search) == StringView::npos)
                continue;

            EngineVariableGraphNode& parent = GetOrCreateParent(localized);
            parent.Children.emplace_back(variable);
        }
    }

    EngineVariablesWindow::EngineVariableGraphNode& EngineVariablesWindow::GetOrCreateParent(StringView nodePath)
    {
        EngineVariableGraphNode* currentNode = &m_Root;
        StringView::size_type currentIndex = -1;
        StringView::size_type previousIndex = 0;
        while ((currentIndex = nodePath.find_first_of( ENGINE_VARIABLE_PATH_SEPERATOR, currentIndex + 1 ) ) != StringView::npos)
        {
            StringView currentLabel = nodePath.substr(previousIndex, ( currentIndex - previousIndex ) );
            bool hasFoundParent = false;
            for (Variant<EngineVariableGraphNode, IEngineVariable*>& child : currentNode->Children)
            {
                EngineVariableGraphNode* parent = std::get_if<EngineVariableGraphNode>(&child);
                if ((parent != nullptr) && IgnoreCaseEqual(currentLabel, parent->Label))
                {
                    currentNode = parent;
                    hasFoundParent = true;
                    break;
                }
            }

            if (hasFoundParent == false)
            {
                Variant<EngineVariableGraphNode, IEngineVariable*>& newParent = currentNode->Children.emplace_back( EngineVariableGraphNode { String(currentLabel) } );
                currentNode = &std::get<EngineVariableGraphNode>(newParent);
            }

            previousIndex = currentIndex + 1; // move past the seperator
        }

        return *currentNode;
    }
}