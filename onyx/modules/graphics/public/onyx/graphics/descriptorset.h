#pragma once

namespace Onyx::Graphics
{
    class Buffer;
    struct TextureHandle;

    class DescriptorSet : public RefCounted
    {
    public:
        DescriptorSet(onyxU8 set) : m_Set(set) {}

        virtual bool HasPendingUpdates() const = 0;

        virtual void UpdateDescriptors() = 0;

        virtual void Bind(const TextureHandle& textureHandle, const String& bindingName) = 0;
        virtual void Bind(const Reference<Buffer>& bufferHandle, const String& bindingName) = 0;
        virtual HashSet<String> GetBindingIds() const = 0;

        onyxU8 GetSet() const { return m_Set; }

    protected:
        onyxS8 m_Set;
    };
}
