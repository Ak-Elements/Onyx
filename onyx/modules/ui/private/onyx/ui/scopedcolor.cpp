#include <onyx/ui/scopedcolor.h>

#if ONYX_USE_IMGUI

#include <imgui.h>

namespace onyx::ui {
ScopedImGuiColor::ScopedImGuiColor( int32_t colorId, uint32_t color ) {
    ImGui::PushStyleColor( colorId, color );
}

ScopedImGuiColor::ScopedImGuiColor( int32_t colorId, Color color ) {
    ImGui::PushStyleColor( colorId, color.toABGR() );
}

ScopedImGuiColor::ScopedImGuiColor( std::initializer_list< std::pair< int32_t, uint32_t > > colorVars ) {
    m_ColorVarsCount = static_cast< int32_t >( colorVars.size() );
    for( const auto& [ colorId, color ] : colorVars ) {
        ImGui::PushStyleColor( colorId, color );
    }
}

ScopedImGuiColor::ScopedImGuiColor( std::initializer_list< std::pair< int32_t, Color > > colorVars ) {
    m_ColorVarsCount = static_cast< int32_t >( colorVars.size() );
    for( const auto& [ colorId, color ] : colorVars ) {
        ImGui::PushStyleColor( colorId, color.toABGR() );
    }
}

ScopedImGuiColor::~ScopedImGuiColor() {
    Reset();
}

void ScopedImGuiColor::Reset() {
    ImGui::PopStyleColor( m_ColorVarsCount );
    m_ColorVarsCount = 0;
}
} // namespace onyx::ui

#endif
