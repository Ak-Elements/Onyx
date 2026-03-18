#include <onyx/filesystem/path.h>
#include <onyx/platform/platformsystem.h>

namespace onyx::platform
{
    PlatformSystem::PlatformSystem(WindowSettings windowSettings, input::InputSystem& inputSystem)
        : m_InputSystem(&inputSystem) 
        , m_Context(*this)
    {
        CreateNewWindow(windowSettings);
    }

    void PlatformSystem::CreateNewWindow(WindowSettings settings)
    {
        UniquePtr<Window>& newWindow = m_Windows.emplace_back(MakeUnique<Window>(m_Context, settings));
        m_WindowCreatedSignal.Dispatch(*newWindow);
    }

    Window& PlatformSystem::GetMainWindow()
    {
        ONYX_ASSERT(m_Windows.empty() == false);
        return *m_Windows[0];
    }

    const Window& PlatformSystem::GetMainWindow() const
    {
        ONYX_ASSERT(m_Windows.empty() == false);
        return *m_Windows[0];
    }

}