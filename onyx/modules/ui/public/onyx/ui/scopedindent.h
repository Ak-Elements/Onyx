#pragma once

#if ONYX_USE_IMGUI

namespace Onyx::Ui
{
    struct ScopedImGuiIndent
    {
        ScopedImGuiIndent();
        ScopedImGuiIndent(onyxF32 indent);
        ~ScopedImGuiIndent();

    private:
        onyxF32 m_Indent = 0.0f;
    };
}

#endif