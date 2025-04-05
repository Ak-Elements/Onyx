#pragma once
#include <imgui.h>

struct ImGuiWindowClass;

namespace Onyx::Ui
{
    enum DockSplitDirection
    {
        Left,
        Right,
        Up,
        Down
    };

    struct DockSplit
    {
        DockSplitDirection Direction;
        onyxF32 Ratio;

        String DockWindowInDirection;
        String DockWindowOppositeDirection;
    };

    class Dockspace
    {
    public:
        static Dockspace Create(const DynamicArray<DockSplit>& splits);
        
        bool Render();

        void SetWindowClass(const ImGuiWindowClass& newWindowClass) { windowClass = &newWindowClass; }
        void Reset() { shouldReset = true; }
        void SetId(onyxU32 newId) { id = newId; }

    private:
        onyxU32 id = 0;
        DynamicArray<DockSplit> splits;
        const ImGuiWindowClass* windowClass;
        bool shouldReset;
    };
}
