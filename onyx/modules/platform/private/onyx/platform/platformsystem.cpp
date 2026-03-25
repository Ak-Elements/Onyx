#include <onyx/filesystem/path.h>
#include <onyx/platform/platformsystem.h>

namespace onyx::platform {
PlatformSystem::PlatformSystem( WindowSettings windowSettings, input::InputSystem& inputSystem )
    : m_InputSystem( &inputSystem )
    , m_Context( *this ) {
    CreateNewWindow( std::move( windowSettings ) );
}

void PlatformSystem::onEndFrame() {
    std::erase_if( m_Windows, [ & ]( const auto& window ) {
        if ( window->getState() == WindowState::Closed ) {
            m_WindowDestroySignal.Dispatch( *window );
            return true;
        }

        return false;
    } );
}

void PlatformSystem::CreateNewWindow( WindowSettings settings ) {
    uint32_t id = static_cast< uint32_t >( m_Windows.size() );
    UniquePtr< Window >& newWindow = m_Windows.emplace_back(
        makeUnique< Window >( id, m_Context, std::move( settings ) ) );
    m_WindowCreatedSignal.Dispatch( *newWindow );
}

Window& PlatformSystem::GetMainWindow() {
    ONYX_ASSERT( m_Windows.empty() == false );
    return *m_Windows[ 0 ];
}

const Window& PlatformSystem::GetMainWindow() const {
    ONYX_ASSERT( m_Windows.empty() == false );
    return *m_Windows[ 0 ];
}

Window& PlatformSystem::GetWindow( uint32_t windowId ) {
    auto windowIt = std::ranges::find_if( m_Windows,
                                          [ & ]( const auto& window ) { return window->getId() == windowId; } );

    ONYX_ASSERT( windowIt != m_Windows.end(), "Tried to configure non existing window" );
    return *( *windowIt );
}

} // namespace onyx::platform