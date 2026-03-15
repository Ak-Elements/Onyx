#pragma once

#include <onyx/ui/propertyinspector.h>
#include <onyx/localization/localizationmodule.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/windows/enginevariableswindow.h>

namespace Onyx::Ui
{
    template <typename T>
    struct PropertyInspector<EngineVariable<T>>
    {
        static bool Draw(EngineVariable<T>& variable, bool /*forceDraw*/)
        {
            StringView localizedPath = g_UiContext.LocalizationSystem->TryGetLocalized(variable.GetId()).value_or(variable.GetId().GetString());

            StringView label = localizedPath;
            StringView::size_type index = localizedPath.find_last_of(EngineVariablesWindow::ENGINE_VARIABLE_PATH_SEPERATOR);
            if(index != StringView::npos)
            {
                label = localizedPath.substr(index + 1);
            }

            if constexpr (Numeric<T> && !std::is_same_v<T, bool>) 
            {
                T value = variable.Get();
                if (PropertyGrid::DrawProperty(label, value, { variable.GetMin(), variable.GetMax() }))
                {
                    variable.Set(value);
                    return true;
                }
            }
            else if constexpr (Invokable<T>)
            {
                if (PropertyGrid::DrawButton(label))
                {
                    variable.Invoke();
                }
            }
            else
            {
                T value = variable.Get();
                if (PropertyGrid::DrawProperty(label, value))
                {
                    variable.Set(value);
                    return true;
                }
            }

            return false;
        }
    };
}