#pragma once

namespace onyx::editor {
struct ICommand {
    ICommand( StringId32 id )
        : m_id( id ) {}

    virtual ~ICommand() = default;

    virtual StringId32 getCommandId() { return m_id; };

    virtual void execute() = 0;

  private:
    StringId32 m_id;
};

template < Invokable Fn > requires Invokable< Fn >
struct Command : public ICommand {
  public:
    Command( StringId32 commandId, Fn executeFunctor )
        : ICommand( commandId )
        , m_execute( executeFunctor ) {}

    void execute() override {
        ONYX_ASSERT( m_execute != nullptr );
        m_execute();
    }

  private:
    std::function< void() > m_execute;
};
} // namespace onyx::editor
