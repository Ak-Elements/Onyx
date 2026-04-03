#pragma once

#if ONYX_USE_IMGUI

#include <onyx/engine/enginesystem.h>
#include <onyx/rhi/graphicshandles.h>
#include <onyx/ui/imguiwindow.h>

namespace onyx::localization {
class LocalizationModule;
}

struct ImFont;
struct ImGuiIO;

namespace onyx {
namespace input {
class InputSystem;
struct MouseAxisEvent;
struct MouseButtonEvent;
struct MousePositionEvent;
struct KeyboardEvent;
struct GameControllerAxisEvent;
struct GameControllerButtonEvent;
} // namespace input

namespace assets {
class AssetSystem;
}

namespace rhi {
class TextureAsset;
struct FrameContext;

class GraphicsSystem;
} // namespace rhi

namespace platform {
class PlatformSystem;
}

namespace ui {
class ImGuiWindow;

template < typename T >
concept IsImGuiWindow = requires( T& window ) {
    // std::is_base_of_v<ImGuiWindow, T>;
    { T::WindowId };
    { T::WindowCategory };
    { window.open() } -> std::same_as< void >;
};

struct ImGuiContext {
    assets::AssetSystem* AssetSystem = nullptr;
    rhi::GraphicsSystem* GraphicsSystem = nullptr;
    input::InputSystem* InputSystem = nullptr;
    localization::LocalizationModule* LocalizationSystem = nullptr;
};

extern ImGuiContext g_UiContext;

class ImGuiSystem : public IEngineSystem {
  public:
    static constexpr StringId32 TypeId = "onyx::ui::ImGuiSystem";
    StringId32 getTypeId() const override { return TypeId; }

    ImGuiSystem( IEngine& engine,
                 assets::AssetSystem& assetSystem,
                 input::InputSystem& inputSystem,
                 localization::LocalizationModule& localizationSystem,
                 rhi::GraphicsSystem& graphicsSystem,
                 platform::PlatformSystem& platformSystem );

    ~ImGuiSystem() override;

    void update( rhi::GraphicsSystem& api, DeltaGameTime deltaTime );

    void OnBeginFrame( const rhi::FrameContext& frameContext );
    void OnRenderFrame( const rhi::FrameContext& frameContext );
    void OnEndFrame( const rhi::FrameContext& frameContext );

    template < IsImGuiWindow T >
    void RegisterWindow() {
        constexpr StringId32 windowTypeId( T::WindowId );
        ONYX_ASSERT( m_WindowFactory.contains( windowTypeId ) == false );
        m_WindowFactory[ windowTypeId ] = [ & ]() {
            auto newWindow = makeUnique< T >();
            newWindow->setEngine( *m_Engine );
            newWindow->setName( String( newWindow->getWindowId() ) );
            return newWindow;
        };

        constexpr StringId32 windowCategory( T::WindowCategory );
        HashSet< StringId32 >& windowsInCategory = m_WindowsPerCategory[ windowCategory ];
        windowsInCategory.emplace( windowTypeId );
    }

    template < IsImGuiWindow T >
    T& OpenWindow( ImGuiWindow& parent ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        constexpr StringId32 windowTypeId( T::WindowId );
        auto factoryIt = m_WindowFactory.find( windowTypeId );

        if( factoryIt == m_WindowFactory.end() ) {
            RegisterWindow< T >();
            factoryIt = m_WindowFactory.find( windowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
        newWindow->setParent( parent );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    template < IsImGuiWindow T >
    T& OpenWindow() {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        constexpr StringId32 windowTypeId( T::WindowId );
        auto factoryIt = m_WindowFactory.find( windowTypeId );
        if( factoryIt == m_WindowFactory.end() ) {
            RegisterWindow< T >();
            factoryIt = m_WindowFactory.find( windowTypeId );
        }

        uint32_t index = static_cast< uint32_t >( m_Windows.size() );
        UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
        newWindow->open();
        return static_cast< T& >( *m_Windows[ index ] );
    }

    template < IsImGuiWindow T >
    T& OpenUniqueWindow() {
        // check if the window is already opened if it is, bring it to the front
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        // create window as its not opened yet
        constexpr StringId32 windowTypeId( T::WindowId );
        auto factoryIt = m_WindowFactory.find( windowTypeId );
        if( factoryIt == m_WindowFactory.end() ) {
            RegisterWindow< T >();
            factoryIt = m_WindowFactory.find( windowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    template < IsImGuiWindow T >
    T& OpenUniqueWindow( ImGuiWindow& parent ) {
        // check if the window is already opened if it is, bring it to the front
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        // create window as its not opened yet
        constexpr StringId32 windowTypeId( T::WindowId );
        auto factoryIt = m_WindowFactory.find( windowTypeId );
        if( factoryIt == m_WindowFactory.end() ) {
            RegisterWindow< T >();
            factoryIt = m_WindowFactory.find( windowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
        newWindow->setParent( parent );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    void OpenWindow( StringId32 windowId ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( StringId32( window->getWindowId() ) == windowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return;
        }

        auto factoryIt = m_WindowFactory.find( windowId );
        if( factoryIt != m_WindowFactory.end() ) {
            UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
            newWindow->open();
        }
    }

    void OpenWindow( ImGuiWindow& parent, StringId32 windowId ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_Windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( StringId32( window->getWindowId() ) == windowId );
        } );

        if( it != m_Windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return;
        }

        auto factoryIt = m_WindowFactory.find( windowId );
        if( factoryIt != m_WindowFactory.end() ) {
            UniquePtr< ImGuiWindow >& newWindow = m_Windows.emplace_back( factoryIt->second() );
            newWindow->setParent( parent );
            newWindow->open();
        }
    }

    void CloseWindow( StringId32 windowId ) {
        ImGuiWindow* window = GetWindow( windowId ).value_or( nullptr );
        if( window ) {
            window->close();
        }
    }

    template < IsImGuiWindow T >
    void CloseWindow() {
        CloseWindow( T::WindowId );
    }

    template < IsImGuiWindow T >
    Optional< T* > GetWindow() {
        Optional< T* > imguiWindow = static_cast< T* >( GetWindow( T::WindowId ).value_or( nullptr ) );
        return imguiWindow;
    }

    Optional< ImGuiWindow* > GetWindow( StringId32 windowName );

    const HashSet< StringId32 >& GetRegisteredWindows( StringId32 category ) {
        return m_WindowsPerCategory.at( category );
    }
    const HashSet< StringId32 >& GetRegisteredWindows( StringId32 category ) const {
        return m_WindowsPerCategory.at( category );
    }

  private:
    void InitRenderBuffers( rhi::GraphicsSystem& graphicsSystem );
    void UpdateDrawBuffers( const rhi::FrameContext& frameContext );

    void OnWindowResize( Vector2s32 size );

    void OnMouseAxisChange( const input::MouseAxisEvent& event );
    void OnMouseButton( const input::MouseButtonEvent& event );
    void OnMousePositionChange( const input::MousePositionEvent& event );

    void OnKey( const input::KeyboardEvent& event );

    void OnControllerAxisChange( const input::GameControllerAxisEvent& event );
    void OnControllerButton( const input::GameControllerButtonEvent& event );

  private:
    Reference< rhi::ShaderInstance > m_ImguiShader;

    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_VertexBuffers;
    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_IndexBuffers;
    rhi::TextureHandle m_FontImage;

    InplaceArray< int32_t, rhi::MAX_FRAMES_IN_FLIGHT > m_VertexCounts;
    InplaceArray< int32_t, rhi::MAX_FRAMES_IN_FLIGHT > m_IndexCounts;

    HashMap< StringId64, ImFont* > m_Fonts;

    HashMap< StringId32, InplaceFunction< UniquePtr< ImGuiWindow >(), 64 > > m_WindowFactory;
    DynamicArray< UniquePtr< ImGuiWindow > > m_Windows;
    HashMap< StringId32, HashSet< StringId32 > > m_WindowsPerCategory;

    IEngine* m_Engine = nullptr;
    platform::PlatformSystem* m_PlatformSystem = nullptr;
    input::InputSystem* m_InputSystem = nullptr;
};
} // namespace ui
} // namespace onyx
#endif
