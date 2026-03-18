#pragma once

namespace onyx::rhi
{
    struct FrameContext;
}

namespace onyx::application
{
    class Application;

    class TaskGraphTask
    {
    public:
        virtual ~TaskGraphTask() = default;
        void Update(onyxU64 deltaTime, rhi::FrameContext& context) { OnUpdate(deltaTime, context); }

    private:
        virtual void OnUpdate(onyxU64 deltaTime, rhi::FrameContext& context) = 0;
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

        void Update(onyxU64 deltaTime, rhi::FrameContext& context);

    private:
#if ONYX_IS_DEBUG
        String m_Name;
#endif

        UniquePtr<TaskGraphTask> m_Task = nullptr;
    };
}
