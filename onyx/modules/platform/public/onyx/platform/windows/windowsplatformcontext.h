#pragma once 

#if ONYX_IS_WINDOWS

#include <onyx/platform/platformsystem.h>

struct HWND__;
typedef HWND__* HWND;

namespace Onyx::Platform
{
    struct WindowsPlatformContext : public IPlatformContext
    {
    public:
        WindowsPlatformContext();

    private:
        HWND m_Instance = nullptr;

    };
}

#endif