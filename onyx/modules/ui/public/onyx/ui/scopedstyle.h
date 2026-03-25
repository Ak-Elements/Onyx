#pragma once

#if ONYX_USE_IMGUI

struct ImVec2;

namespace onyx::ui {
struct ScopedImGuiStyle {
  private:
    using StyleVarValue = Variant< float32, Vector2f32, ImVec2 >; // To handle float or Vector2f32 values

  public:
    ScopedImGuiStyle( int32_t styleVar, float32 value );
    ScopedImGuiStyle( int32_t styleVar, const Vector2f32& value );
    ScopedImGuiStyle( int32_t styleVar, const ImVec2& value );
    // list of style vars
    ScopedImGuiStyle( std::initializer_list< std::pair< int32_t, StyleVarValue > > styleVars );

    ~ScopedImGuiStyle();

    void Reset();

  private:
    int32_t m_StyleVarsCount = 1;
};
} // namespace onyx::ui
#endif