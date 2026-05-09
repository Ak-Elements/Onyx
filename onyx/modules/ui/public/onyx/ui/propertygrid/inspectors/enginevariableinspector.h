#pragma once

#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/propertyinspector.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/windows/enginevariableswindow.h>

namespace onyx::ui {
template < typename T >
struct PropertyInspector< EngineVariable< T > > {
    static bool draw( EngineVariable< T >& variable, bool /*forceDraw*/ ) {
        StringView localizedPath = g_uiContext.LocalizationSystem->TryGetLocalized( variable.getId() )
                                       .value_or( variable.getId().getString() );

        StringView label = localizedPath;
        StringView::size_type index = localizedPath.find_last_of( EngineVariablesWindow::EngineVariablePathSeperator );
        if( index != StringView::npos ) {
            label = localizedPath.substr( index + 1 );
        }

        if constexpr( Numeric< T > && !std::is_same_v< T, bool > ) {
            T value = variable.get();
            if( property_grid::drawProperty( label, value, { variable.getMin(), variable.getMax() } ) ) {
                variable.set( value );
                return true;
            }
        } else if constexpr( std::is_invocable_v< T > ) {
            if( property_grid::drawButton( label ) ) {
                variable.Invoke();
            }
        } else {
            T value = variable.get();
            if( property_grid::drawProperty( label, value ) ) {
                variable.set( value );
                return true;
            }
        }

        return false;
    }
};
} // namespace onyx::ui
