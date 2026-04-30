#include <onyx/ui/windows/fpsstatusbaritem.h>

#if ONYX_USE_IMGUI

namespace onyx::ui {

void FpsStatusBarItem::update( uint64_t deltaTime ) {
    m_averageFrameTime += ( numericCast< float64 >( deltaTime ) - m_averageFrameTime ) * 0.03f;
    uint64_t avgFps = static_cast< uint64_t >( 1000.0 / m_averageFrameTime );
    m_value = format::format( "{}({:.2} ms)", avgFps, m_averageFrameTime );
}

} // namespace onyx::ui

#endif
