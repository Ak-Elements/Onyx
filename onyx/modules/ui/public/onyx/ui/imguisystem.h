#pragma once

#if ONYX_USE_IMGUI

#include <onyx/ui/imguiwindow.h>

#include <onyx/engine/enginesystem.h>

#include <onyx/thread/container/lockfreempscboundedqueue.h>


#include <mutex>

namespace Onyx::Localization
{
    class LocalizationModule;
}

struct ImFont;
struct ImGuiIO;

namespace Onyx
{
    namespace Input
    {
        struct InputEvent;
        class InputSystem;
    }

    namespace Assets
    {
        class AssetSystem;
    }

    namespace Graphics
    {
        class TextureAsset;
        struct FrameContext;

        class GraphicsSystem;
        class Window;
        class WindowSystem;
    }

    namespace Ui
    {
        class ImGuiWindow;

        template <typename T>
        concept IsImGuiWindow = requires(T& window)
        {
            //std::is_base_of_v<ImGuiWindow, T>;
            { T::WindowId };
            { window.Open() } -> std::same_as<void>;
        };

        struct ImGuiContext
        {
            Assets::AssetSystem* AssetSystem = nullptr;
            Graphics::GraphicsSystem* GraphicsSystem = nullptr;
            Graphics::Window* MainWindow = nullptr;
            Input::InputSystem* InputSystem = nullptr;
        };

        extern ImGuiContext g_UiContext;

        class ImGuiSystem : public IEngineSystem
        {
        public:
            static constexpr StringId32 TypeId = "Onyx::Ui::ImGuiModule";
            StringId32 GetTypeId() const override { return TypeId; }

            static Reference<Graphics::TextureAsset> FolderClosedAsset;
            static Reference<Graphics::TextureAsset> FolderOpenAsset;
            static Reference<Graphics::TextureAsset> FolderSelectedClosedAsset;
            static Reference<Graphics::TextureAsset> FolderSelectedOpenAsset;

            ImGuiSystem(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem, Graphics::WindowSystem& windowSystem);
            ~ImGuiSystem() override;

            
            void Update(Graphics::GraphicsSystem& api, DeltaGameTime deltaTime);

            void OnBeginFrame(Graphics::FrameContext& context);
            
            void OnEndFrame();

            template <IsImGuiWindow T, typename... Args>
            void RegisterWindow(Args&&... args)
            {
                constexpr onyxU32 windowTypeId = TypeHash<T>();
                ONYX_ASSERT(m_WindowFactory.contains(windowTypeId) == false);
                m_WindowFactory[windowTypeId] = [&]()
                {
                    return MakeUnique<T>(std::forward<Args>(args)...);
                };
            }

            template <IsImGuiWindow T, typename... Args>
            T& OpenWindow(Args&&... args)
            {
                // find first non open window matching the id and reuse
                auto it = std::ranges::find_if(m_Windows, [&](const UniquePtr<ImGuiWindow>& window)
                    {
                        return (window->IsOpen() == false) && (window->GetWindowId() == T::WindowId);
                    });

                if (it != m_Windows.end())
                {
                    UniquePtr<ImGuiWindow>& imguiWindow = *it;
                    imguiWindow->Open();
                    return static_cast<T&>(*imguiWindow);
                }

                constexpr onyxU32 windowTypeId = TypeHash<T>();
                auto factoryIt = m_WindowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == m_WindowFactory.end())
                {
                    if constexpr (std::is_constructible<T, Args...>())
                    {
                        RegisterWindow<T>(std::forward<Args>(args)...);
                        newWindow = MakeUnique<T>(std::forward<Args>(args)...);
                    }
                    else
                    {
                        ONYX_ASSERT(false, "Window is not registered and not constructible with the given arguments");
                    }
                }
                else
                {
                    newWindow = factoryIt->second();
                }

                newWindow->SetName(String(newWindow->GetWindowId()));
                newWindow->Open();
                T& newWindowRef = static_cast<T&>(*newWindow);
                m_Windows.push_back(std::move(newWindow));
                return newWindowRef;
            }

            template <IsImGuiWindow T, typename... Args>
            T& OpenUniqueWindow(Args&&... args)
            {
                // check if the window is already opened if it is, bring it to the front
                auto it = std::ranges::find_if(m_Windows, [&](const UniquePtr<ImGuiWindow>& window)
                    {
                        return (window->GetWindowId() == T::WindowId);
                    });

                if (it != m_Windows.end())
                {
                    UniquePtr<ImGuiWindow>& imguiWindow = *it;
                    imguiWindow->Open();
                    return static_cast<T&>(*imguiWindow);
                }

                // create window as its not opened yet
                constexpr onyxU32 windowTypeId = TypeHash<T>();
                auto factoryIt = m_WindowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == m_WindowFactory.end())
                {
                    if constexpr (std::is_constructible<T, Args...>())
                    {
                        RegisterWindow<T>(std::forward<Args>(args)...);
                        newWindow = MakeUnique<T>(std::forward<Args>(args)...);
                    }
                    else
                    {
                        ONYX_ASSERT(false, "Window is not registered and not constructible with the given arguments");
                    }
                }
                else
                {
                    newWindow = factoryIt->second();
                }

                newWindow->SetName(String(newWindow->GetWindowId()));
                newWindow->Open();
                T& newWindowRef = static_cast<T&>(*newWindow);
                m_Windows.push_back(std::move(newWindow));
                return newWindowRef;
            }

            template <IsImGuiWindow T>
            void CloseWindow()
            {
                CloseWindow(T::WindowId);
            }

            template <IsImGuiWindow T>
            Optional<T*> GetWindow()
            {
                Optional<T*> imguiWindow = static_cast<T*>(GetWindow(T::WindowId).value_or(nullptr));
                return imguiWindow;
            }

            //void ShowWindow(StringView windowName);
            //void CloseWindow(StringView windowName);
            Optional<ImGuiWindow*> GetWindow(StringView windowName);
          
        private:
            void OnWindowResize(onyxU32 width, onyxU32 height);
            void OnInputEvent(const Input::InputEvent* event);

        private:
            LockFreeMPSCBoundedQueue<InplaceFunction<void(ImGuiIO&)>, 64> m_QueuedInputs;
            HashMap<onyxU32, InplaceFunction<UniquePtr<ImGuiWindow>(), 64>> m_WindowFactory;
            HashMap<StringId64, ImFont*> m_Fonts;
            std::mutex m_Mutex;
            DynamicArray<UniquePtr<ImGuiWindow>> m_Windows;
            Graphics::Window* m_Window;
            Input::InputSystem* m_InputSystem;
        };
    }
}
#endif