#pragma once

#if ONYX_USE_IMGUI

struct ImVec2;

namespace onyx::ui
{
    struct ScopedImGuiStyle
    {
    private:
        using StyleVarValue = Variant<onyxF32, Vector2f32, ImVec2>;  // To handle float or Vector2f32 values

    public:
        ScopedImGuiStyle(onyxS32 styleVar, onyxF32 value);
        ScopedImGuiStyle(onyxS32 styleVar, const Vector2f32& value);
        ScopedImGuiStyle(onyxS32 styleVar, const ImVec2& value);
        // list of style vars
        ScopedImGuiStyle(std::initializer_list<std::pair<onyxS32, StyleVarValue>> styleVars);

        ~ScopedImGuiStyle();

        void Reset();

    private:
        onyxS32 m_StyleVarsCount = 1;
    };
}
#endif