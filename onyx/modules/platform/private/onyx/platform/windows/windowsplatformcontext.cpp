#include <onyx/platform/windows/windowsplatformcontext.h>

#if ONYX_IS_WINDOWS

#include <onyx/platform/platformsystem.h>

#include <Windows.h>

namespace Onyx::Platform::Windows
{
    PlatformContext::PlatformContext(PlatformSystem& platformSystem)
        : m_PlatformSystem(&platformSystem)
        , m_Instance(::GetModuleHandle(nullptr))
    {

    }

    Input::InputSystem& PlatformContext::GetInputSystem()
    {
        ONYX_ASSERT(m_PlatformSystem != nullptr);
        return m_PlatformSystem->GetInputSystem();
    }
}

#endif