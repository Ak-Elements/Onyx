#pragma once

#if ONYX_USE_IMGUI

namespace onyx::ui {
struct ScopedImGuiColor {
  public:
    ScopedImGuiColor( int32_t colorId, uint32_t color );

    // list of color vars
    ScopedImGuiColor( std::initializer_list< std::pair< int32_t, uint32_t > > colorVars );

    ~ScopedImGuiColor();

    void Reset();

  private:
    int32_t m_ColorVarsCount = 1;
};
} // namespace onyx::ui
#endif