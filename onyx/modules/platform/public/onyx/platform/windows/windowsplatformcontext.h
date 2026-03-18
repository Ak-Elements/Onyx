#pragma once 

#if ONYX_IS_WINDOWS

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

namespace onyx::input
{
    class InputSystem;
}

namespace onyx::platform
{
    class PlatformSystem;
    namespace windows
    {
        class PlatformContext
        {
        public:
            PlatformContext(PlatformSystem& platformSystem);

            input::InputSystem& GetInputSystem();
            HINSTANCE GetInstanceHandle() const { return m_Instance; }

        private:
            PlatformSystem* m_PlatformSystem = nullptr;
            HINSTANCE m_Instance = nullptr;
        };
    }
}

#endif