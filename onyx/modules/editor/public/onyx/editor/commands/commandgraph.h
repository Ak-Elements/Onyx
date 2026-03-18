#pragma once

#include <onyx/editor/commands/command.h>

namespace onyx::editor
{
    class ICommandGraph
    {
    public:
        virtual ~ICommandGraph() = default;
        virtual void MoveForward() = 0;
        virtual void MoveBack() = 0;
        virtual void Reset(const ICommand& command) = 0;

        template <typename U, typename... Args> requires std::is_base_of_v<ICommand, U>
        void Push(Args&&... args)
        {
            DoPush(MakeUnique<U>(std::forward<Args>(args)...));
        }

        virtual const DynamicArray<UniquePtr<ICommand>>& GetCommands() const = 0;

    private:
        virtual void DoPush(UniquePtr<ICommand>&& command) = 0;
    };

    template <typename T>// requires std::is_trivially_copyable_v<T>
    class CommandGraph : public ICommandGraph
    {
        struct Branch
        {
            DynamicArray<UniquePtr<ICommand>> m_Commands;
            onyxS32 m_CurrentCommandIndex = INVALID_INDEX_32;

            onyxS32 GetCount() { return numeric_cast<onyxS32>(m_Commands.size()); }

            void Push(UniquePtr<ICommand>&& command)
            {
                command->Execute();
                m_Commands.emplace_back(std::move(command));
                ++m_CurrentCommandIndex;
            }

            void Reset(const ICommand& command, const T& base, T& head)
            {
                ONYX_ASSERT(m_Commands.empty() == false);

                auto it = std::ranges::find_if(m_Commands, [&](const UniquePtr<ICommand>& c)
                {
                    return &command == c.get();
                });

                ONYX_ASSERT(it != m_Commands.end());
                onyxS32 index = numeric_cast<onyxS32>(std::distance(m_Commands.begin(), it));
                
                if (index > m_CurrentCommandIndex)
                {
                    MoveForward(index - m_CurrentCommandIndex);
                }
                else
                {
                    MoveBack((m_CurrentCommandIndex - index), base, head);
                }
            }

            void MoveForward(onyxS32 count)
            {
                ONYX_ASSERT((m_CurrentCommandIndex + count) <= (GetCount() - 1));
                
                for ( onyxS32 i = 1; i <= count; ++i )
                {
                    const UniquePtr<ICommand>& command = m_Commands[m_CurrentCommandIndex + i];
                    command->Execute();
                }

                m_CurrentCommandIndex += count;
            }

            void MoveBack(onyxS32 count, const T& base, T& head)
            {
                ONYX_ASSERT((m_CurrentCommandIndex - count) >= INVALID_INDEX_32);

                head = base;
                m_CurrentCommandIndex -= count;
                for ( onyxS32 i = 0; i <= m_CurrentCommandIndex; ++i )
                {
                    const UniquePtr<ICommand>& command = m_Commands[i];
                    command->Execute();
                }
            }
        };

    public:
        ~CommandGraph() override = default;

        void MoveForward() override
        {
            ONYX_ASSERT(m_Head != nullptr);
            m_Current.MoveForward(1);
        }

        void MoveBack() override
        {
            ONYX_ASSERT(m_Head != nullptr);
            m_Current.MoveBack(1, m_Base, *m_Head);
        }

        void Reset(const ICommand& command) override
        {
            ONYX_ASSERT(m_Head != nullptr);
            m_Current.Reset(command, m_Base, *m_Head);
        }

        const DynamicArray<UniquePtr<ICommand>>& GetCommands() const override { return m_Current.m_Commands; }

        void SetBase(const T& base) { m_Base = base; }
        void SetHead(T& head) { m_Head = &head; }

    private:
        void DoPush(UniquePtr<ICommand>&& command) override
        {
            m_Current.Push(std::move(command));
        }

    private:
        Branch m_Current;
        
        T* m_Head = nullptr;
        T m_Base;
    };
}