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
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    PlatformSystem( WindowSettings windowSettings, input::InputSystem& inputSystem );

    void onEndFrame();

    PlatformContext& getContext() { return m_context; }
    [[nodiscard]] const PlatformContext& getContext() const { return m_context; }

    input::InputSystem& getInputSystem() {
        ONYX_ASSERT( m_inputSystem != nullptr );
        return *m_inputSystem;
    }

    void createNewWindow( WindowSettings settings );
    Window& getMainWindow();
    [[nodiscard]] bool hasWindows() const { return m_windows.empty() == false; }
    [[nodiscard]] const Window& getMainWindow() const;

    [[nodiscard]] Window& getWindow( uint32_t windowId );
    [[nodiscard]] const DynamicArray< UniquePtr< Window > >& getWindows() const { return m_windows; }

    template < auto Candidate, typename Type > void onWindowCreate( Type* instance ) {
        Sink sink( m_windowCreatedSignal );
        sink.template connect< Candidate >( instance );
    }

    template < auto Candidate, typename Type > void onWindowDestroy( Type* instance ) {
        Sink sink( m_windowDestroySignal );
        sink.template connect< Candidate >( instance );
    }

    template < typename Type > void disconnectSignals( Type* instance ) {
        Sink createdSignalSink( m_windowCreatedSignal );
        Sink destroySignalSink( m_windowDestroySignal );
        createdSignalSink.disconnect( instance );
        destroySignalSink.disconnect( instance );
    }

  private:
    void onWindowClose( uint32_t windowId );

  private:
    input::InputSystem* m_inputSystem = nullptr;

    PlatformContext m_context;
    DynamicArray< UniquePtr< Window > > m_windows;

    WindowCreatedSignalT m_windowCreatedSignal;
    WindowDestroySignalT m_windowDestroySignal;
};
} // namespace onyx::platform
