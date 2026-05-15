#pragma once

#include <onyx/editor/commands/command.h>

namespace onyx::editor {
class ICommandGraph {
  public:
    virtual ~ICommandGraph() = default;
    virtual void moveForward() = 0;
    virtual void moveBack() = 0;
    virtual void reset( const ICommand& command ) = 0;

    template < typename U, typename... Args > requires std::is_base_of_v< ICommand, U >
    void push( Args&&... args ) {
        doPush( makeUnique< U >( std::forward< Args >( args )... ) );
    }

    [[nodiscard]] virtual const DynamicArray< UniquePtr< ICommand > >& getCommands() const = 0;

  private:
    virtual void doPush( UniquePtr< ICommand >&& command ) = 0;
};

template < typename T > // requires std::is_trivially_copyable_v<T>
class CommandGraph : public ICommandGraph {
    class Branch {
      public:
        [[nodiscard]] const DynamicArray< UniquePtr< ICommand > >& getCommands() const { return m_commands; }
        int32_t getCount() { return numericCast< int32_t >( m_commands.size() ); }

        void push( UniquePtr< ICommand >&& command ) {
            command->execute();
            m_commands.emplace_back( std::move( command ) );
            ++m_currentCommandIndex;
        }

        void reset( const ICommand& command, const T& base, T& head ) {
            ONYX_ASSERT( m_commands.empty() == false );

            auto it = std::ranges::find_if( m_commands,
                                            [ & ]( const UniquePtr< ICommand >& c ) { return &command == c.get(); } );

            ONYX_ASSERT( it != m_commands.end() );
            int32_t index = numericCast< int32_t >( std::distance( m_commands.begin(), it ) );

            if( index > m_currentCommandIndex ) {
                moveForward( index - m_currentCommandIndex );
            } else {
                moveBack( ( m_currentCommandIndex - index ), base, head );
            }
        }

        void moveForward( int32_t count ) {
            ONYX_ASSERT( ( m_currentCommandIndex + count ) <= ( getCount() - 1 ) );

            for( int32_t i = 1; i <= count; ++i ) {
                const UniquePtr< ICommand >& command = m_commands[ m_currentCommandIndex + i ];
                command->execute();
            }

            m_currentCommandIndex += count;
        }

        void moveBack( int32_t count, const T& base, T& head ) {
            ONYX_ASSERT( ( m_currentCommandIndex - count ) >= InvalidIndex32 );

            head = base;
            m_currentCommandIndex -= count;
            for( int32_t i = 0; i <= m_currentCommandIndex; ++i ) {
                const UniquePtr< ICommand >& command = m_commands[ i ];
                command->execute();
            }
        }

      private:
        DynamicArray< UniquePtr< ICommand > > m_commands;
        int32_t m_currentCommandIndex = InvalidIndex32;
    };

  public:
    ~CommandGraph() override = default;

    void moveForward() override {
        ONYX_ASSERT( m_head != nullptr );
        m_current.moveForward( 1 );
    }

    void moveBack() override {
        ONYX_ASSERT( m_head != nullptr );
        m_current.moveBack( 1, m_base, *m_head );
    }

    void reset( const ICommand& command ) override {
        ONYX_ASSERT( m_head != nullptr );
        m_current.reset( command, m_base, *m_head );
    }

    [[nodiscard]] const DynamicArray< UniquePtr< ICommand > >& getCommands() const override {
        return m_current.getCommands();
    }

    void setBase( const T& base ) { m_base = base; }
    void setHead( T& head ) { m_head = &head; }

  private:
    void doPush( UniquePtr< ICommand >&& command ) override { m_current.push( std::move( command ) ); }

  private:
    Branch m_current;

    T* m_head = nullptr;
    T m_base;
};
} // namespace onyx::editor
