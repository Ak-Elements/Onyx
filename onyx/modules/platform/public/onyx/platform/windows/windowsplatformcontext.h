#pragma once 

#if ONYX_IS_WINDOWS

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

namespace Onyx::Input
{
    class InputSystem;
}

namespace Onyx::Platform
{
    class PlatformSystem;
    namespace Windows
    {
        class PlatformContext
        {
        public:
            PlatformContext(PlatformSystem& platformSystem);

            Input::InputSystem& GetInputSystem();
            HINSTANCE GetInstanceHandle() const { return m_Instance; }

        private:
            PlatformSystem* m_PlatformSystem = nullptr;
            HINSTANCE m_Instance = nullptr;
        };
    }
}

#endif