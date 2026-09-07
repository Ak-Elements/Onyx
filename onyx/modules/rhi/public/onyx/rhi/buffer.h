#pragma once

#include <onyx/rhi/bufferproperties.h>

namespace onyx::rhi {
enum class Access : uint32_t;
enum class Context : uint8_t;
class CommandBuffer;

class Buffer : public RefCounted {
  public:
    Buffer( const BufferProperties& properties );

    void* getData() { return m_dataPointer; }
    virtual void setData( uint64_t offset, const void* data, uint64_t length ) = 0;

    [[nodiscard]] const BufferProperties& getProperties() const { return m_properties; }

    [[nodiscard]] bool isMapped() const { return m_dataPointer != nullptr; }

#if !ONYX_IS_RETAIL
    [[nodiscard]] bool hasGpuAddress() const {
        return enums::isSet( m_properties.m_UsageFlags, BufferUsage::DeviceAddress ) && m_gpuAddress != 0;
    }
#endif

    [[nodiscard]] uint64_t getGpuAddress() const { return m_gpuAddress; }
    [[nodiscard]] virtual uint64_t getAliasOffset( int8_t alias ) const = 0;
    [[nodiscard]] virtual uint64_t getAliasSize( int8_t alias ) const = 0;

    virtual void* map( MapMode mode ) = 0;
    virtual void unmap() = 0;

    virtual void flush( uint32_t offset, uint32_t count ) = 0;

    virtual void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess ) = 0;
    virtual void barrier( CommandBuffer& commandBuffer, Context newContext, Access newAccess, int8_t aliasIndex ) = 0;
    virtual int8_t alias( const BufferProperties& properties ) = 0;
    virtual void clearAliases() = 0;

  protected:
    const BufferProperties m_properties;

    uint64_t m_gpuAddress = 0;
    void* m_dataPointer = nullptr;
};
} // namespace onyx::rhi
