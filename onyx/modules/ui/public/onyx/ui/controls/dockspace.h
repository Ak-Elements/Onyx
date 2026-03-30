#pragma once
#include <imgui.h>

struct ImGuiWindowClass;

namespace onyx::ui {
enum class DockSplitDirection : uint8_t { Left, Right, Up, Down };

struct DockSplit {
    DockSplitDirection Direction;
    float32 Ratio;

    String DockWindowInDirection;
    String DockWindowOppositeDirection;
};

class Dockspace {
  public:
    void init( uint32_t id, const ImGuiWindowClass* windowClass, const DynamicArray< DockSplit >& splits );

    void render();

  private:
    uint32_t m_id = 0;
    const ImGuiWindowClass* m_windowClass = nullptr;
};
} // namespace onyx::ui
