#pragma once

namespace Onyx::Graphics
{
    struct FrameContext;
}

namespace Onyx::Application
{
    class Application;

    class TaskGraphTask
    {
    public:
        virtual ~TaskGraphTask() = default;
        void Update(onyxU64 deltaTime, Graphics::FrameContext& context) { OnUpdate(deltaTime, context); }

    private:
        virtual void OnUpdate(onyxU64 deltaTime, Graphics::FrameContext& context) = 0;
    };

    class TaskGraphNode
    {
    public:
        TaskGraphNode() = default;
#if ONYX_IS_DEBUG
        TaskGraphNode(const String& name);
#endif
        TaskGraphNode(UniquePtr<TaskGraphTask>&& task);

        TaskGraphNode(TaskGraphNode&& other) noexcept;
        TaskGraphNode& operator=(TaskGraphNode&& other) noexcept;

        virtual ~TaskGraphNode() = default;

        void Update(onyxU64 deltaTime, Graphics::FrameContext& context);

    private:
#if ONYX_IS_DEBUG
        String m_Name;
#endif

        UniquePtr<TaskGraphTask> m_Task = nullptr;
    };
}
