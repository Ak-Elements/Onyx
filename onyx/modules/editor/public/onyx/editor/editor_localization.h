#pragma once

#include <onyx/localization/localizedstring.h>

namespace Onyx::Localization
{
    class LocalizationModule;

    namespace Editor
    {
        // Window Titles
        namespace Windows
        {
            inline LocalizedString StartupTitle;
            inline LocalizedString NodeEditorTitle;
            inline LocalizedString ShaderGraphEditorTitle;
            inline LocalizedString RenderGraphEditorTitle;
            inline LocalizedString VolumeShaderGraphEditorTitle;
        }

        // Main Menubar
        namespace MainMenubar
        {
            namespace View
            {
                inline LocalizedString Label;

                namespace Layouts
                {
                    inline LocalizedString Label;
                }
            }

            namespace Project
            {
                inline LocalizedString Label;
            }

            namespace Debug
            {
                inline LocalizedString Label;
            }
        }

        // Scene Editor
        namespace SceneEditor
        {
            inline LocalizedString Title;
            inline LocalizedString SceneViewport;
        }

        /// Components Panel
        namespace ComponentsPanel
        {
            inline LocalizedString Title;
            inline LocalizedString ShowAll;
        }

        namespace EntitiesPanel
        {
            inline LocalizedString Title;
        }

        namespace InputActionSettings
        {
            inline LocalizedString Title;
            inline LocalizedString AddAction;

            namespace Bindings
            {
                inline LocalizedString Unbound;
                inline LocalizedString Listen;
            }
        }

        namespace NodeEditor
        {
            inline LocalizedString Title;
            inline LocalizedString UnnamedGraph;

            inline LocalizedString CreateNode;
            inline LocalizedString ReplaceNode;

            inline LocalizedString CreateLink;
            inline LocalizedString ReplaceLink;

            namespace MainMenubar
            {
                namespace Debug
                {
                    inline LocalizedString Label;
                    inline LocalizedString ShowLinkDirections;
                }
            }

            namespace Error
            {
                inline LocalizedString ConnectSamePin;
                inline LocalizedString ConnectSelf;
                inline LocalizedString IncompatiblePinType;
                inline LocalizedString TwoInputPins;
                inline LocalizedString TwoOutputPins;
                inline LocalizedString DependencyCycle;
            }



        }

        void InitLocalization(LocalizationModule& localizationModule);

    }
}