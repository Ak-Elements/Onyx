#include <onyx/filesystem/path.h>
#include <onyx/platform/platformsystem.h>

namespace onyx::platform {
PlatformSystem::PlatformSystem( WindowSettings windowSettings, input::InputSystem& inputSystem )
    : m_inputSystem( &inputSystem )
    , m_context( *this ) {
    createNewWindow( std::move( windowSettings ) );
}

void PlatformSystem::onEndFrame() {
    std::erase_if( m_windows, [ & ]( const auto& window ) {
        if( window->getState() == WindowState::Closed ) {
            m_windowDestroySignal.dispatch( *window );
            return true;
        }

        return false;
    } );
}

void PlatformSystem::createNewWindow( WindowSettings settings ) {
    uint32_t id = static_cast< uint32_t >( m_windows.size() );
    UniquePtr< Window >& newWindow = m_windows.emplace_back(
        makeUnique< Window >( id, m_context, std::move( settings ) ) );
    m_windowCreatedSignal.dispatch( *newWindow );
}

Window& PlatformSystem::getMainWindow() {
    ONYX_ASSERT( m_windows.empty() == false );
    return *m_windows[ 0 ];
}

const Window& PlatformSystem::getMainWindow() const {
    ONYX_ASSERT( m_windows.empty() == false );
    return *m_windows[ 0 ];
}

Window& PlatformSystem::getWindow( uint32_t windowId ) {
    auto windowIt = std::ranges::find_if( m_windows,
                                          [ & ]( const auto& window ) { return window->getId() == windowId; } );

    ONYX_ASSERT( windowIt != m_windows.end(), "Tried to configure non existing window" );
    return *( *windowIt );
}

} // namespace onyx::platform
