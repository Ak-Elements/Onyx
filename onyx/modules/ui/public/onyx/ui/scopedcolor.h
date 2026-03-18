#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui
{
    struct ScopedImGuiColor
    {
    public:
        ScopedImGuiColor(onyxS32 colorId, onyxU32 color);

        // list of color vars
        ScopedImGuiColor(std::initializer_list<std::pair<onyxS32, onyxU32>> colorVars);

        ~ScopedImGuiColor();

        void Reset();

    private:
        onyxS32 m_ColorVarsCount = 1;
    };
}
#endif