#include <onyx/platform/windows/windowsplatformcontext.h>

#if ONYX_IS_WINDOWS

#include <onyx/platform/platformsystem.h>

#include <Windows.h>

namespace onyx::platform::windows
{
    PlatformContext::PlatformContext(PlatformSystem& platformSystem)
        : m_PlatformSystem(&platformSystem)
        , m_Instance(::GetModuleHandle(nullptr))
    {

    }

    input::InputSystem& PlatformContext::GetInputSystem()
    {
        ONYX_ASSERT(m_PlatformSystem != nullptr);
        return m_PlatformSystem->GetInputSystem();
    }
}

#endif