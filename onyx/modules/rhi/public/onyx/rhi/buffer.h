#pragma once

#include <onyx/rhi/bufferproperties.h>

namespace onyx::rhi {
enum class Access : uint32_t;
enum class Context : uint8_t;
class CommandBuffer;

class Buffer : public RefCounted {
  public:
    Buffer( const BufferProperties& properties );

    void* GetData() { return m_DataPointer; }
    virtual void SetData( int32_t offset, const void* data, int32_t length ) = 0;

    const BufferProperties& GetProperties() const { return m_Properties; }

    bool IsMapped() const { return m_DataPointer != nullptr; }

    uint64_t GetGpuAddress() const { return m_GpuAddress; }
    virtual uint64_t GetAliasOffset( int8_t alias ) const = 0;
    virtual uint64_t GetAliasSize( int8_t alias ) const = 0;

    virtual void* Map( MapMode mode ) = 0;
    virtual void Unmap() = 0;

    virtual void Flush( uint32_t offset, uint32_t count ) = 0;

    virtual void Barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) = 0;
    virtual void Barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) = 0;
    virtual int8_t Alias( const BufferProperties& properties ) = 0;
    virtual void ClearAliases() = 0;

  protected:
    const BufferProperties m_Properties;

    uint64_t m_GpuAddress = 0;
    void* m_DataPointer = nullptr;
};
} // namespace onyx::rhi
