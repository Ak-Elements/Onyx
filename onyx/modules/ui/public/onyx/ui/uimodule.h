#pragma once
#include <onyx/engine/enginesystem.h>

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::Ui
{
    class UiModule : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Ui::UiModule";
        StringId32 GetTypeId() const override { return TypeId; }

        UiModule(Localization::LocalizationModule& localizationModule);
    };
}
