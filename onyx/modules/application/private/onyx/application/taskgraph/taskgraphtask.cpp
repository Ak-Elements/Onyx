#include <onyx/application/taskgraph/taskgraphtask.h>
#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Application
{

#if ONYX_IS_DEBUG
    TaskGraphNode::TaskGraphNode(const String& name)
        : m_Name(name)
    {
    }
#endif

    TaskGraphNode::TaskGraphNode(UniquePtr<TaskGraphTask>&& task)
#if ONYX_IS_DEBUG
        : m_Name("")
        , m_Task(std::move(task))
#else
        : m_Task(std::move(task))
#endif
    {
    }

    TaskGraphNode::TaskGraphNode(TaskGraphNode&& other) noexcept
        : m_Task(std::move(other.m_Task))
    {
    }

    TaskGraphNode& TaskGraphNode::operator=(TaskGraphNode&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_Task = std::move(other.m_Task);
        return *this;
    }

    void TaskGraphNode::Update(onyxU64 deltaTime, Graphics::FrameContext& context)
    {
        ONYX_ASSERT(m_Task != nullptr);
        m_Task->Update(deltaTime, context);
    }
}
