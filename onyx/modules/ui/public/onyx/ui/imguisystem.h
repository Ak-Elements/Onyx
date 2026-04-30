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
class Theme;

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

extern ImGuiContext g_uiContext;

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

    void update( rhi::GraphicsSystem& graphicsSystem, DeltaGameTime deltaTime );

    void onBeginFrame( const rhi::FrameContext& frameContext );
    void onRenderFrame( const rhi::FrameContext& frameContext );
    void onEndFrame( const rhi::FrameContext& frameContext );

    template < IsImGuiWindow T >
    void registerWindow() {
        constexpr StringId32 WindowTypeId( T::WindowId );
        ONYX_ASSERT( m_windowFactory.contains( WindowTypeId ) == false );
        m_windowFactory[ WindowTypeId ] = [ & ]() {
            auto newWindow = makeUnique< T >();
            newWindow->setEngine( *m_engine );
            newWindow->setName( String( newWindow->getWindowId() ) );
            return newWindow;
        };

        constexpr StringId32 WindowCategory( T::WindowCategory );
        HashSet< StringId32 >& windowsInCategory = m_windowsPerCategory[ WindowCategory ];
        windowsInCategory.emplace( WindowTypeId );
    }

    template < IsImGuiWindow T >
    T& openWindow( ImGuiWindow& parent ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        constexpr StringId32 WindowTypeId( T::WindowId );
        auto factoryIt = m_windowFactory.find( WindowTypeId );

        if( factoryIt == m_windowFactory.end() ) {
            registerWindow< T >();
            factoryIt = m_windowFactory.find( WindowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
        newWindow->setParent( parent );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    template < IsImGuiWindow T >
    T& openWindow() {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        constexpr StringId32 WindowTypeId( T::WindowId );
        auto factoryIt = m_windowFactory.find( WindowTypeId );
        if( factoryIt == m_windowFactory.end() ) {
            registerWindow< T >();
            factoryIt = m_windowFactory.find( WindowTypeId );
        }

        uint32_t index = static_cast< uint32_t >( m_windows.size() );
        UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
        newWindow->open();
        return static_cast< T& >( *m_windows[ index ] );
    }

    template < IsImGuiWindow T >
    T& openUniqueWindow() {
        // check if the window is already opened if it is, bring it to the front
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        // create window as its not opened yet
        constexpr StringId32 WindowTypeId( T::WindowId );
        auto factoryIt = m_windowFactory.find( WindowTypeId );
        if( factoryIt == m_windowFactory.end() ) {
            registerWindow< T >();
            factoryIt = m_windowFactory.find( WindowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    template < IsImGuiWindow T >
    T& openUniqueWindow( ImGuiWindow& parent ) {
        // check if the window is already opened if it is, bring it to the front
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->getWindowId() == T::WindowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return static_cast< T& >( *imguiWindow );
        }

        // create window as its not opened yet
        constexpr StringId32 WindowTypeId( T::WindowId );
        auto factoryIt = m_windowFactory.find( WindowTypeId );
        if( factoryIt == m_windowFactory.end() ) {
            registerWindow< T >();
            factoryIt = m_windowFactory.find( WindowTypeId );
        }

        UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
        newWindow->setParent( parent );
        newWindow->open();
        return static_cast< T& >( *newWindow );
    }

    void openWindow( StringId32 windowId ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( StringId32( window->getWindowId() ) == windowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->open();
            return;
        }

        auto factoryIt = m_windowFactory.find( windowId );
        if( factoryIt != m_windowFactory.end() ) {
            UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
            newWindow->open();
        }
    }

    void openWindow( ImGuiWindow& parent, StringId32 windowId ) {
        // find first non open window matching the id and reuse
        auto it = std::ranges::find_if( m_windows, [ & ]( const UniquePtr< ImGuiWindow >& window ) {
            return ( window->isOpen() == false ) && ( StringId32( window->getWindowId() ) == windowId );
        } );

        if( it != m_windows.end() ) {
            UniquePtr< ImGuiWindow >& imguiWindow = *it;
            imguiWindow->setParent( parent );
            imguiWindow->open();
            return;
        }

        auto factoryIt = m_windowFactory.find( windowId );
        if( factoryIt != m_windowFactory.end() ) {
            UniquePtr< ImGuiWindow >& newWindow = m_windows.emplace_back( factoryIt->second() );
            newWindow->setParent( parent );
            newWindow->open();
        }
    }

    void closeWindow( StringId32 windowId ) {
        ImGuiWindow* window = getWindow( windowId ).value_or( nullptr );
        if( window ) {
            window->close();
        }
    }

    template < IsImGuiWindow T >
    void closeWindow() {
        closeWindow( T::WindowId );
    }

    template < IsImGuiWindow T >
    Optional< T* > getWindow() {
        Optional< T* > imguiWindow = static_cast< T* >( getWindow( T::WindowId ).value_or( nullptr ) );
        return imguiWindow;
    }

    Optional< ImGuiWindow* > getWindow( StringId32 windowName );

    const HashSet< StringId32 >& getRegisteredWindows( StringId32 category ) {
        return m_windowsPerCategory.at( category );
    }
    const HashSet< StringId32 >& getRegisteredWindows( StringId32 category ) const {
        return m_windowsPerCategory.at( category );
    }

    const Theme& getTheme() const;

  private:
    void initRenderBuffers( rhi::GraphicsSystem& graphicsSystem );
    void updateDrawBuffers( const rhi::FrameContext& frameContext );

    void onWindowResize( Vector2s32 size );

    void onMouseAxisChange( const input::MouseAxisEvent& event );
    void onMouseButton( const input::MouseButtonEvent& event );
    void onMousePositionChange( const input::MousePositionEvent& event );

    void onKey( const input::KeyboardEvent& event );

    void onControllerAxisChange( const input::GameControllerAxisEvent& event );
    void onControllerButton( const input::GameControllerButtonEvent& event );

    void onThemeLoaded( assets::AssetHandle< Theme > loadedTheme );

  private:
    Reference< rhi::ShaderInstance > m_imguiShader;
    assets::AssetHandle< Theme > m_activeTheme;

    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_vertexBuffers;
    InplaceArray< rhi::BufferHandle, rhi::MAX_FRAMES_IN_FLIGHT > m_indexBuffers;
    rhi::TextureHandle m_fontImage;

    InplaceArray< int32_t, rhi::MAX_FRAMES_IN_FLIGHT > m_vertexCounts;
    InplaceArray< int32_t, rhi::MAX_FRAMES_IN_FLIGHT > m_indexCounts;

    HashMap< StringId64, ImFont* > m_fonts;

    HashMap< StringId32, InplaceFunction< UniquePtr< ImGuiWindow >(), 64 > > m_windowFactory;
    DynamicArray< UniquePtr< ImGuiWindow > > m_windows;
    HashMap< StringId32, HashSet< StringId32 > > m_windowsPerCategory;

    IEngine* m_engine = nullptr;
    platform::PlatformSystem* m_platformSystem = nullptr;
    input::InputSystem* m_inputSystem = nullptr;
};
} // namespace ui
} // namespace onyx
#endif
