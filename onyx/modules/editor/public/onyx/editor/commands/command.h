#pragma once

namespace Onyx::Editor
{
    struct ICommand
    {
        ICommand(StringId32 id)
            : m_Id(id)
        {
        }

        virtual ~ICommand() = default;

        virtual StringId32 GetCommandId() { return m_Id; };

        virtual void Execute() = 0;

    private:
        StringId32 m_Id;
    };

    template <Invokable Fn> requires Invokable<Fn>
    struct Command : public ICommand
    {
    public:
        Command(StringId32 commandId, Fn executeFunctor)
            : ICommand(commandId)
            , m_Execute(executeFunctor)
        {
        }
        
        void Execute() override 
        {
            ONYX_ASSERT(m_Execute != nullptr);
            m_Execute();
        }

    private:
        std::function<void()> m_Execute;
    };
}