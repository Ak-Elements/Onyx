#pragma once
#include "imguiwindow.h"

#if ONYX_USE_IMGUI

#include <onyx/engine/enginesystem.h>

#include <onyx/thread/container/lockfreempscboundedqueue.h>
#include <onyx/graphics/textureasset.h>

#include <mutex>

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
        struct FrameContext;
        class GraphicsApi;
        class Window;
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

        class ImGuiSystem : public IEngineSystem
        {
        public:
            static Reference<Graphics::TextureAsset> FolderClosedAsset;
            static Reference<Graphics::TextureAsset> FolderOpenAsset;
            static Reference<Graphics::TextureAsset> FolderSelectedClosedAsset;
            static Reference<Graphics::TextureAsset> FolderSelectedOpenAsset;

            ImGuiSystem();
            ~ImGuiSystem() override;

            void Init(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem, Graphics::Window& window);
            void Shutdown(Input::InputSystem& inputSystem, Graphics::Window& window);
            
            void Update(onyxU64 deltaTime);

            void OnBeginFrame(Graphics::FrameContext& context);
            
            void OnEndFrame();

            template <IsImGuiWindow T, typename... Args>
            void RegisterWindow(Args&&... args)
            {
                constexpr onyxU32 windowTypeId = TypeHash<T>();
                ONYX_ASSERT(windowFactory.contains(windowTypeId) == false);
                windowFactory[windowTypeId] = [&]()
                {
                    return MakeUnique<T>(std::forward<Args>(args)...);
                };
            }

            template <IsImGuiWindow T, typename... Args>
            T& OpenWindow(Args&&... args)
            {
                // find first non open window matching the id and reuse
                auto it = std::ranges::find_if(windows, [&](const UniquePtr<ImGuiWindow>& window)
                    {
                        return (window->IsOpen() == false) && (window->GetWindowId() == T::WindowId);
                    });

                if (it != windows.end())
                {
                    UniquePtr<ImGuiWindow>& imguiWindow = *it;
                    imguiWindow->Open();
                    return static_cast<T&>(*imguiWindow);
                }

                constexpr onyxU32 windowTypeId = TypeHash<T>();
                auto factoryIt = windowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == windowFactory.end())
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
                    newWindow = std::move(factoryIt->second());
                }

                newWindow->SetName(String(newWindow->GetWindowId()));
                newWindow->Open();
                T& newWindowRef = static_cast<T&>(*newWindow);
                windows.push_back(std::move(newWindow));
                return newWindowRef;
            }

            template <IsImGuiWindow T, typename... Args>
            T& OpenUniqueWindow(Args&&... args)
            {
                // check if the window is already opened if it is, bring it to the front
                auto it = std::ranges::find_if(windows, [&](const UniquePtr<ImGuiWindow>& window)
                    {
                        return (window->GetWindowId() == T::WindowId);
                    });

                if (it != windows.end())
                {
                    UniquePtr<ImGuiWindow>& imguiWindow = *it;
                    imguiWindow->Open();
                    return static_cast<T&>(*imguiWindow);
                }

                // create window as its not opened yet
                constexpr onyxU32 windowTypeId = TypeHash<T>();
                auto factoryIt = windowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == windowFactory.end())
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
                    newWindow = std::move(factoryIt->second());
                }

                newWindow->SetName(String(newWindow->GetWindowId()));
                newWindow->Open();
                T& newWindowRef = static_cast<T&>(*newWindow);
                windows.push_back(std::move(newWindow));
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
            Graphics::Window* window;
            HashMap<onyxU64, ImFont*> fonts;
            std::mutex mutex;
            LockFreeMPSCBoundedQueue<InplaceFunction<void(ImGuiIO&)>, 64> queuedInputs;

            DynamicArray<UniquePtr<ImGuiWindow>> windows;
            HashMap<onyxU32, InplaceFunction<UniquePtr<ImGuiWindow>()>> windowFactory;
        };
    }
}
#endif