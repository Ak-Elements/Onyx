#pragma once

#if ONYX_USE_IMGUI

#include <onyx/engine/enginesystem.h>
#include <onyx/ui/imguiwindow.h>
#include <onyx/rhi/graphicshandles.h>

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
        class InputSystem;
        struct MouseAxisEvent;
        struct MouseButtonEvent;
        struct MousePositionEvent;
        struct KeyboardEvent;
        struct GameControllerAxisEvent;
        struct GameControllerButtonEvent;
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

    namespace Platform
    {
        class PlatformSystem;
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

            ImGuiSystem(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem, Graphics::GraphicsSystem& graphicsSystem, Platform::PlatformSystem& platformSystem);
            ~ImGuiSystem() override;

            void Update(Graphics::GraphicsSystem& api, DeltaGameTime deltaTime);

            void OnBeginFrame(const Graphics::FrameContext& frameContext);
            void OnRenderFrame(const Graphics::FrameContext& frameContext);
            void OnEndFrame(const Graphics::FrameContext& frameContext);

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
            void InitRenderBuffers(Graphics::GraphicsSystem& graphicsSystem);
            void UpdateDrawBuffers(const Graphics::FrameContext& frameContext);

            void OnWindowResize(onyxU32 width, onyxU32 height);
            
            void OnMouseAxisChange(const Input::MouseAxisEvent& event);
            void OnMouseButton(const Input::MouseButtonEvent& event);
            void OnMousePositionChange(const Input::MousePositionEvent& event);

            void OnKey(const Input::KeyboardEvent& event);

            void OnControllerAxisChange(const Input::GameControllerAxisEvent& event);
            void OnControllerButton(const Input::GameControllerButtonEvent& event);

        private:
            Reference<Graphics::ShaderInstance> m_ImguiShader;

            InplaceArray<Graphics::BufferHandle, Graphics::MAX_FRAMES_IN_FLIGHT> m_VertexBuffers;
            InplaceArray<Graphics::BufferHandle, Graphics::MAX_FRAMES_IN_FLIGHT> m_IndexBuffers;
            Graphics::TextureHandle m_FontImage;

            InplaceArray<onyxS32, Graphics::MAX_FRAMES_IN_FLIGHT> m_VertexCounts;
            InplaceArray<onyxS32, Graphics::MAX_FRAMES_IN_FLIGHT> m_IndexCounts;

            HashMap<onyxU32, InplaceFunction<UniquePtr<ImGuiWindow>(), 64>> m_WindowFactory;
            HashMap<StringId64, ImFont*> m_Fonts;
            std::mutex m_Mutex;
            DynamicArray<UniquePtr<ImGuiWindow>> m_Windows;
            Platform::PlatformSystem* m_PlatformSystem;
            Input::InputSystem* m_InputSystem;
        };
    }
}
#endif