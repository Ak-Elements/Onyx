#pragma once

#include <onyx/engine/enginesystem.h>

namespace onyx
{
    namespace localization
    {
        class LocalizationModule;
    }
    
    namespace ui
    {
        class ImGuiSystem;
    }
}

namespace onyx::editor
{
    class EditorSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "onyx::editor::EditorSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        EditorSystem(ui::ImGuiSystem& imguiSystem, localization::LocalizationModule& localizationModule);

        ~EditorSystem() override;
    };
}
