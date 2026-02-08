#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx
{
    namespace Localization
    {
        class LocalizationModule;
    }
    
    namespace Ui
    {
        class ImGuiSystem;
    }
}

namespace Onyx::Editor
{
    class EditorSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Editor::EditorModule";
        StringId32 GetTypeId() const override { return TypeId; }

        EditorSystem(Ui::ImGuiSystem& imguiSystem,
            Localization::LocalizationModule& localizationModule);

        ~EditorSystem() override;
    };
}
