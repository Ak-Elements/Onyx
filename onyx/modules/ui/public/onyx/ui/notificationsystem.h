#pragma once

#include <onyx/engine/enginesystem.h>

namespace onyx::ui {
class NotificationSystem : public IEngineSystem {
  public:
    static constexpr StringId32 TypeId = "onyx::ui::NotificationSystem";
    StringId32 getTypeId() const override { return TypeId; }

    void update();
};
} // namespace onyx::ui
