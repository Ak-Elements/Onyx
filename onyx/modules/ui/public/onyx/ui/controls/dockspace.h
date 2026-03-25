#pragma once
#include <imgui.h>

struct ImGuiWindowClass;

namespace onyx::ui {
enum DockSplitDirection { Left, Right, Up, Down };

struct DockSplit {
    DockSplitDirection Direction;
    float32 Ratio;

    String DockWindowInDirection;
    String DockWindowOppositeDirection;
};

class Dockspace {
  public:
    static Dockspace Create( const DynamicArray< DockSplit >& splits );

    bool Render();

    void SetWindowClass( const ImGuiWindowClass& newWindowClass ) { windowClass = &newWindowClass; }
    void Reset() { shouldReset = true; }
    void SetId( uint32_t newId ) { id = newId; }

  private:
    uint32_t id = 0;
    DynamicArray< DockSplit > splits;
    const ImGuiWindowClass* windowClass;
    bool shouldReset;
};
} // namespace onyx::ui
