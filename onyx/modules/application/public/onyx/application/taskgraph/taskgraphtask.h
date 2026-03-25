#pragma once

namespace onyx::rhi {
struct FrameContext;
}

namespace onyx::application {
class Application;

class TaskGraphTask {
  public:
    virtual ~TaskGraphTask() = default;
    void update( uint64_t deltaTime, rhi::FrameContext& context ) { onUpdate( deltaTime, context ); }

  private:
    virtual void onUpdate( uint64_t deltaTime, rhi::FrameContext& context ) = 0;
};

class TaskGraphNode {
  public:
    TaskGraphNode() = default;
#if ONYX_IS_DEBUG
    explicit TaskGraphNode( const String& name );
#endif
    explicit TaskGraphNode( UniquePtr< TaskGraphTask >&& task );

    TaskGraphNode( TaskGraphNode&& other ) noexcept;
    TaskGraphNode& operator=( TaskGraphNode&& other ) noexcept;

    virtual ~TaskGraphNode() = default;

    void update( uint64_t deltaTime, rhi::FrameContext& context );

  private:
#if ONYX_IS_DEBUG
    String m_name;
#endif

    UniquePtr< TaskGraphTask > m_task = nullptr;
};
} // namespace onyx::application
