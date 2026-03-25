#pragma once

#include <onyx/engine/enginesystem.h>
#include <onyx/platform/platformcontext.h>

#include <onyx/platform/window.h>

namespace onyx::input {
class InputSystem;
}

namespace onyx::platform {
class PlatformSystem : public IEngineSystem {
  public:
    using WindowCreatedSignalT = Signal< void( const Window& ) >;
    using WindowDestroySignalT = Signal< void( const Window& ) >;

    static constexpr StringId32 TypeId{ "onyx::platform::PlatformSystem" };
    StringId32 getTypeId() const override { return TypeId; }

    PlatformSystem( WindowSettings windowSettings, input::InputSystem& inputSystem );

    void onEndFrame();

    PlatformContext& GetContext() { return m_Context; }
    const PlatformContext& GetContext() const { return m_Context; }

    input::InputSystem& GetInputSystem() {
        ONYX_ASSERT( m_InputSystem != nullptr );
        return *m_InputSystem;
    }

    void CreateNewWindow( WindowSettings settings );
    Window& GetMainWindow();
    bool hasWindows() const { return m_Windows.empty() == false; }
    ONYX_NO_DISCARD const Window& GetMainWindow() const;

    ONYX_NO_DISCARD Window& GetWindow( uint32_t windowId );
    ONYX_NO_DISCARD const DynamicArray< UniquePtr< Window > >& GetWindows() const { return m_Windows; }

    template < auto Candidate, typename Type > void onWindowCreate( Type* instance ) {
        Sink sink( m_WindowCreatedSignal );
        sink.template Connect< Candidate >( instance );
    }

    template < auto Candidate, typename Type > void onWindowDestroy( Type* instance ) {
        Sink sink( m_WindowDestroySignal );
        sink.template Connect< Candidate >( instance );
    }

    template < typename Type > void DisconnectSignals( Type* instance ) {
        Sink createdSignalSink( m_WindowCreatedSignal );
        Sink destroySignalSink( m_WindowDestroySignal );
        createdSignalSink.Disconnect( instance );
        destroySignalSink.Disconnect( instance );
    }

  private:
    void onWindowClose( uint32_t windowId );

  private:
    input::InputSystem* m_InputSystem = nullptr;

    PlatformContext m_Context;
    DynamicArray< UniquePtr< Window > > m_Windows;

    WindowCreatedSignalT m_WindowCreatedSignal;
    WindowDestroySignalT m_WindowDestroySignal;
};
} // namespace onyx::platform