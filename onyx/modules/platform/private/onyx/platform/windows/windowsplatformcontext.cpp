#include <onyx/platform/windows/windowsplatformcontext.h>

#if ONYX_IS_WINDOWS

namespace Onyx::Platform
{
    WindowsPlatformContext::WindowsPlatformContext()
        : m_Instance(::GetModuleHandle(nullptr))
    {

    }

}

#endif