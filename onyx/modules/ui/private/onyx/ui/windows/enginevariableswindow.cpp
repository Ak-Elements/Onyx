#include <onyx/ui/windows/enginevariableswindow.h>

#include <onyx/localization/localizationmodule.h>

#include <onyx/ui/propertygrid.h>
#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/widgets.h>

namespace onyx::ui {
void EngineVariablesWindow::onOpen() {
    rebuildVariableGraph();
}

void EngineVariablesWindow::onRender( ImGuiSystem& /*imguiSystem*/ ) {
    bool hasSearchChanged = drawSearchBar( m_search, "Search", m_searchBarIsFocused );
    if( hasSearchChanged ) {
        rebuildVariableGraph();
    }

    property_grid::beginPropertyGrid( "Engine Vars", 80 );

    renderGraphNode( m_root );
    property_grid::endPropertyGrid();
}

void EngineVariablesWindow::renderGraphNode( const EngineVariablesWindow::EngineVariableGraphNode& graphNode ) const {
    const bool isSearching = m_search.empty() == false;
    ImGuiTreeNodeFlags flags = isSearching ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;

    const bool isRoot = &graphNode == &m_root;
    bool isOpen = true;
    if( isRoot == false ) {
        isOpen = property_grid::beginCollapsiblePropertyGroup( graphNode.Label, flags );
    }

    if( isOpen ) {
        for( const Variant< EngineVariableGraphNode, IEngineVariable* >& child : graphNode.Children ) {
            if( const EngineVariableGraphNode* parent = std::get_if< EngineVariableGraphNode >( &child ) ) {
                renderGraphNode( *parent );
            } else if( IEngineVariable* variable = std::get< IEngineVariable* >( child ) ) {
                PropertyInspectors::draw( variable->getRuntimeTypeId(), variable, false );
            }
        }

        if( isRoot == false ) {
            property_grid::endPropertyGroup();
        }
    }
}

void EngineVariablesWindow::rebuildVariableGraph() {
    m_root.Children.clear();

    const localization::LocalizationModule& localizationSystem = getEngineSystem< localization::LocalizationModule >();
    HashMap< StringId32, IEngineVariable* > engineVariables = IEngine::getVariablesRegistry().getVariables();

    for( auto&& [ id, variable ] : engineVariables ) {
        StringView localized = localizationSystem.TryGetLocalized( id ).value_or( id.getString() );
        if( ignoreCaseFind( localized, m_search ) == StringView::npos )
            continue;

        EngineVariableGraphNode& parent = getOrCreateParent( localized );
        parent.Children.emplace_back( variable );
    }
}

EngineVariablesWindow::EngineVariableGraphNode& EngineVariablesWindow::getOrCreateParent( StringView nodePath ) {
    EngineVariableGraphNode* currentNode = &m_root;
    StringView::size_type currentIndex = static_cast< StringView::size_type >( -1 );
    StringView::size_type previousIndex = 0;
    while( ( currentIndex = nodePath.find_first_of( EngineVariablePathSeperator, currentIndex + 1 ) ) !=
           StringView::npos ) {
        StringView currentLabel = nodePath.substr( previousIndex, ( currentIndex - previousIndex ) );
        bool hasFoundParent = false;
        for( Variant< EngineVariableGraphNode, IEngineVariable* >& child : currentNode->Children ) {
            EngineVariableGraphNode* parent = std::get_if< EngineVariableGraphNode >( &child );
            if( ( parent != nullptr ) && ignoreCaseEqual( currentLabel, parent->Label ) ) {
                currentNode = parent;
                hasFoundParent = true;
                break;
            }
        }

        if( hasFoundParent == false ) {
            Variant< EngineVariableGraphNode, IEngineVariable* >& newParent = currentNode->Children.emplace_back(
                EngineVariableGraphNode{ String( currentLabel ) } );
            currentNode = &std::get< EngineVariableGraphNode >( newParent );
        }

        previousIndex = currentIndex + 1; // move past the seperator
    }

    return *currentNode;
}
} // namespace onyx::ui
