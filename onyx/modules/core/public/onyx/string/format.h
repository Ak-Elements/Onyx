#pragma once

#include <onyx/container/memorybuffer.h>
#include <onyx/container/span.h>
#include <onyx/string/inplacestring.h>

#include <format>

namespace Onyx
{
    namespace Format
    {
        static constexpr size_t DEFAULT_BUFFER_SIZE = 8192;

        using StringMemoryBufferContext = MemoryRingBufferBase<char>;

        template <onyxU32 Size, onyxU8 Alignment>
        using StringMemoryBuffer = MemoryRingBuffer<char, Size, Alignment>;

        static StringMemoryBufferContext* GetStringBufferThreadContext()
        {
            static __declspec(thread) std::unique_ptr<StringMemoryBufferContext> gs_StringBufferTLS(new StringMemoryBuffer<DEFAULT_BUFFER_SIZE, sizeof(onyxU32)>);
            return gs_StringBufferTLS.get();
        }

        inline Span<char> GetNextStringBuffer()
        {
            return GetStringBufferThreadContext()->GetBuffer();
        }

        inline Span<char> GetNextStringBuffer(onyxU32 size)
        {
            return GetStringBufferThreadContext()->GetBuffer(size);
        }

        template <onyxU32 Size, typename... Args>
        void FormatTo2(InplaceString<Size>& buffer, StringView fmt, Args&&... args)
        {
            std::vformat_to(buffer.GetData(), fmt, std::forward<Args>(args)...);
        }

        template <onyxU32 Size,typename... Args>
        void FormatTo(InplaceString<Size>& buffer, std::format_string<Args...> fmt, Args&&... args)
        {
            std::format_to_n_result result = std::format_to_n(buffer.GetData(), Size, fmt, std::forward<Args>(args)...);
            *result.out = '\0';
        }

        template <typename... Args>
        void FormatTo(const Span<char>& buffer, std::format_string<Args...> fmt, Args&&... args)
        {
            std::format_to_n_result result = std::format_to_n(buffer.data(), buffer.size(), fmt, std::forward<Args>(args)...);
            *result.out = '\0';
        }

        template <typename... Args>
        const char* Format(std::format_string<Args...> fmt, Args&&... args)
        {
            onyxU64 formattedSize = std::formatted_size(fmt, std::forward<Args>(args)...);

            if (formattedSize > DEFAULT_BUFFER_SIZE)
            {
                assert(false);
                //ONYX_ASSERT(false, "");
            }

            const Span<char> formatBuffer = GetNextStringBuffer();
            FormatTo(formatBuffer, fmt, std::forward<Args>(args)...);
            return formatBuffer.data();
        }
    }
}
