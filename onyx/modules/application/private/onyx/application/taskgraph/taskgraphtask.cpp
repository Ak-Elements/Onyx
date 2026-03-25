#include <onyx/application/taskgraph/taskgraphtask.h>

namespace onyx::application {

#if ONYX_IS_DEBUG
TaskGraphNode::TaskGraphNode( const String& name )
    : m_name( name ) {}
#endif

TaskGraphNode::TaskGraphNode( UniquePtr< TaskGraphTask >&& task )
#if ONYX_IS_DEBUG
    : m_task( std::move( task ) )
#else
    : m_Task( std::move( task ) )
#endif
{
}

TaskGraphNode::TaskGraphNode( TaskGraphNode&& other ) noexcept
    : m_task( std::move( other.m_task ) ) {}

TaskGraphNode& TaskGraphNode::operator=( TaskGraphNode&& other ) noexcept {
    if ( this == &other )
        return *this;

    m_task = std::move( other.m_task );
    return *this;
}

void TaskGraphNode::update( uint64_t deltaTime, rhi::FrameContext& context ) {
    ONYX_ASSERT( m_task != nullptr );
    m_task->update( deltaTime, context );
}
} // namespace onyx::application
