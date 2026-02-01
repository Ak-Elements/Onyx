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
            { T::WindowCategory };
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

            ImGuiSystem(IEngine& engine, Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem, Graphics::GraphicsSystem& graphicsSystem, Platform::PlatformSystem& platformSystem);
            ~ImGuiSystem() override;

            void Update(Graphics::GraphicsSystem& api, DeltaGameTime deltaTime);

            void OnBeginFrame(const Graphics::FrameContext& frameContext);
            void OnRenderFrame(const Graphics::FrameContext& frameContext);
            void OnEndFrame(const Graphics::FrameContext& frameContext);

            template <IsImGuiWindow T>
            void RegisterWindow()
            {
                constexpr StringId32 windowTypeId(T::WindowId);
                ONYX_ASSERT(m_WindowFactory.contains(windowTypeId) == false);
                m_WindowFactory[windowTypeId] = []() { return MakeUnique<T>(); };

                constexpr StringId32 windowCategory(T::WindowCategory);
                HashSet<StringId32>& windowsInCategory = m_WindowsPerCategory[windowCategory];
                windowsInCategory.emplace(windowTypeId);
            }

            template <IsImGuiWindow T>
            T& OpenWindow()
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

                constexpr StringId32 windowTypeId(T::WindowId);
                auto factoryIt = m_WindowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == m_WindowFactory.end())
                {
                    RegisterWindow<T>();
                    newWindow = MakeUnique<T>();
                }
                else
                {
                    newWindow = factoryIt->second();
                }

                newWindow->SetEngine(*m_Engine);
                newWindow->SetName(String(newWindow->GetWindowId()));
                newWindow->Open();
                T& newWindowRef = static_cast<T&>(*newWindow);
                m_Windows.push_back(std::move(newWindow));
                return newWindowRef;
            }

            template <IsImGuiWindow T>
            T& OpenUniqueWindow()
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
                constexpr StringId32 windowTypeId(T::WindowId);
                auto factoryIt = m_WindowFactory.find(windowTypeId);
                UniquePtr<ImGuiWindow> newWindow;
                if (factoryIt == m_WindowFactory.end())
                { 
                    RegisterWindow<T>();
                    newWindow = MakeUnique<T>();
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

            void OpenWindow(StringId32 windowId)
            {
                // find first non open window matching the id and reuse
                auto it = std::ranges::find_if(m_Windows, [&](const UniquePtr<ImGuiWindow>& window)
                    {
                        return (window->IsOpen() == false) && (StringId32(window->GetWindowId()) == windowId);
                    });

                if (it != m_Windows.end())
                {
                    UniquePtr<ImGuiWindow>& imguiWindow = *it;
                    imguiWindow->Open();
                    return;
                }

                auto factoryIt = m_WindowFactory.find(windowId);
                if (factoryIt != m_WindowFactory.end())
                {
                    UniquePtr<ImGuiWindow>& newWindow = m_Windows.emplace_back(factoryIt->second());
                    newWindow->SetName(String(newWindow->GetWindowId()));
                    newWindow->Open();
                }
            }

            void CloseWindow(StringId32 windowId)
            {
                ImGuiWindow* window = GetWindow(windowId).value_or(nullptr);
                if (window)
                {
                    window->Close();
                }
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
            Optional<ImGuiWindow*> GetWindow(StringId32 windowName);
          
            const HashSet<StringId32>& GetRegisteredWindows(StringId32 category) { return m_WindowsPerCategory.at(category); }
            const HashSet<StringId32>& GetRegisteredWindows(StringId32 category) const { return m_WindowsPerCategory.at(category); }

        private:
            void InitRenderBuffers(Graphics::GraphicsSystem& graphicsSystem);
            void UpdateDrawBuffers(const Graphics::FrameContext& frameContext);

            void OnWindowResize(Vector2s32 size);
            
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

            HashMap<StringId64, ImFont*> m_Fonts;
            
            HashMap<StringId32, InplaceFunction<UniquePtr<ImGuiWindow>(), 64>> m_WindowFactory;
            DynamicArray<UniquePtr<ImGuiWindow>> m_Windows;
            HashMap<StringId32, HashSet<StringId32>> m_WindowsPerCategory;
            
            IEngine* m_Engine = nullptr;
            Platform::PlatformSystem* m_PlatformSystem = nullptr;
            Input::InputSystem* m_InputSystem = nullptr;
        };
    }
}
#endif