#include <onyx/platform/platformsystem.h>

namespace Onyx::Platform
{
    PlatformSystem::PlatformSystem(Input::InputSystem& inputSystem)
        : m_InputSystem(&inputSystem) 
        , m_Context(*this)
    {
        WindowSettings defaultSettings;
        CreateNewWindow(defaultSettings);
    }

    void PlatformSystem::CreateNewWindow(WindowSettings settings)
    {
        m_Windows.emplace_back(MakeUnique<Window>(m_Context, settings));
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