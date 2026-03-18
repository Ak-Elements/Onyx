#pragma once
#include <onyx/engine/enginesystem.h>

namespace onyx::localization
{
    class LocalizationModule;
}

namespace onyx::ui
{
    class UiModule : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "onyx::ui::UiSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        UiModule(onyx::localization::LocalizationModule& localizationModule);
    };
}
