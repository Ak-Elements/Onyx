#pragma once

#include <onyx/editor/commands/command.h>

namespace onyx::editor {
class ICommandGraph {
  public:
    virtual ~ICommandGraph() = default;
    virtual void MoveForward() = 0;
    virtual void MoveBack() = 0;
    virtual void Reset( const ICommand& command ) = 0;

    template < typename U, typename... Args > requires std::is_base_of_v< ICommand, U >
    void Push( Args&&... args ) {
        DoPush( makeUnique< U >( std::forward< Args >( args )... ) );
    }

    virtual const DynamicArray< UniquePtr< ICommand > >& GetCommands() const = 0;

  private:
    virtual void DoPush( UniquePtr< ICommand >&& command ) = 0;
};

template < typename T > // requires std::is_trivially_copyable_v<T>
class CommandGraph : public ICommandGraph {
    struct Branch {
        DynamicArray< UniquePtr< ICommand > > m_Commands;
        int32_t m_CurrentCommandIndex = InvalidIndex32;

        int32_t GetCount() { return numericCast< int32_t >( m_Commands.size() ); }

        void Push( UniquePtr< ICommand >&& command ) {
            command->Execute();
            m_Commands.emplace_back( std::move( command ) );
            ++m_CurrentCommandIndex;
        }

        void Reset( const ICommand& command, const T& base, T& head ) {
            ONYX_ASSERT( m_Commands.empty() == false );

            auto it = std::ranges::find_if( m_Commands,
                                            [ & ]( const UniquePtr< ICommand >& c ) { return &command == c.get(); } );

            ONYX_ASSERT( it != m_Commands.end() );
            int32_t index = numericCast< int32_t >( std::distance( m_Commands.begin(), it ) );

            if ( index > m_CurrentCommandIndex ) {
                MoveForward( index - m_CurrentCommandIndex );
            } else {
                MoveBack( ( m_CurrentCommandIndex - index ), base, head );
            }
        }

        void MoveForward( int32_t count ) {
            ONYX_ASSERT( ( m_CurrentCommandIndex + count ) <= ( GetCount() - 1 ) );

            for ( int32_t i = 1; i <= count; ++i ) {
                const UniquePtr< ICommand >& command = m_Commands[ m_CurrentCommandIndex + i ];
                command->Execute();
            }

            m_CurrentCommandIndex += count;
        }

        void MoveBack( int32_t count, const T& base, T& head ) {
            ONYX_ASSERT( ( m_CurrentCommandIndex - count ) >= InvalidIndex32 );

            head = base;
            m_CurrentCommandIndex -= count;
            for ( int32_t i = 0; i <= m_CurrentCommandIndex; ++i ) {
                const UniquePtr< ICommand >& command = m_Commands[ i ];
                command->Execute();
            }
        }
    };

  public:
    ~CommandGraph() override = default;

    void MoveForward() override {
        ONYX_ASSERT( m_Head != nullptr );
        m_Current.MoveForward( 1 );
    }

    void MoveBack() override {
        ONYX_ASSERT( m_Head != nullptr );
        m_Current.MoveBack( 1, m_Base, *m_Head );
    }

    void Reset( const ICommand& command ) override {
        ONYX_ASSERT( m_Head != nullptr );
        m_Current.Reset( command, m_Base, *m_Head );
    }

    const DynamicArray< UniquePtr< ICommand > >& GetCommands() const override { return m_Current.m_Commands; }

    void SetBase( const T& base ) { m_Base = base; }
    void SetHead( T& head ) { m_Head = &head; }

  private:
    void DoPush( UniquePtr< ICommand >&& command ) override { m_Current.Push( std::move( command ) ); }

  private:
    Branch m_Current;

    T* m_Head = nullptr;
    T m_Base;
};
} // namespace onyx::editor