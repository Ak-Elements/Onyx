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
        void Init(Localization::LocalizationModule& localizationModule);
    };
}
