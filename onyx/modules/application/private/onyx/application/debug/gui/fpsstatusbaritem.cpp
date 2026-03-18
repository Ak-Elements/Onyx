#include <onyx/application/debug/gui/fpsstatusbaritem.h>

#if ONYX_UI_MODULE && ONYX_USE_IMGUI

namespace onyx::application::debug {

void FpsStatusBarItem::Update( onyxU64 deltaTime ) {
    AverageFrameTime += ( numeric_cast< onyxF64 >( deltaTime ) - AverageFrameTime ) * 0.03f;
    onyxU64 avgFps = static_cast< onyxU64 >( 1000.0 / AverageFrameTime );
    Value = format::Format( "{}({:.2} ms)", avgFps, AverageFrameTime );
}

} // namespace onyx::application::debug

#endif
