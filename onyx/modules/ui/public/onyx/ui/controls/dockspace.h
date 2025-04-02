#pragma once

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
        
        bool Render(onyxU32 dockspaceId);

        void SetWindowClass(const ImGuiWindowClass& newWindowClass) { windowClass = &newWindowClass; }
        void Reset() { shouldReset = true; }

    private:
        DynamicArray<DockSplit> splits;
        const ImGuiWindowClass* windowClass;
        bool shouldReset;
    };
}
