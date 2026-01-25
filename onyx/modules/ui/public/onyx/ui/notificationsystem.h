#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx::Ui
{
    class NotificationSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Ui::NotificationSystem";
        StringId32 GetTypeId() const override { return TypeId; }

        void Update();
    };
}
