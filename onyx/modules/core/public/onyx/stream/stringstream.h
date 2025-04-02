#pragma once

#include <algorithm>
#include <string>
#include <charconv>

#include <onyx/onyx_types.h>
#include <onyx/stream/stream.h>

namespace Onyx
{
    class StringStream : public Stream
    {
    public:
        StringStream() = default;
        explicit StringStream(const String& str);

        bool IsValid() const override { return m_Data.empty(); }
        bool IsEof() const override { return m_CurrentDataPosition >= GetLength(); }
        onyxU64 GetPosition() override { return m_CurrentDataPosition; }
        onyxU64 GetPosition() const override { return m_CurrentDataPosition; }
        void SetPosition(onyxU64 position) override { m_CurrentDataPosition = position; }
        onyxU64 GetLength() const override { return static_cast<onyxU64>(m_Data.size()); }

        const String& GetData() { return m_Data; }

        void SkipWhitespaces();

        bool ReadString(StringView& outStrView, const char delimiter);
        bool ReadString(StringView& outStrView);
        bool ReadLine(StringView& outStrView);
        bool ReadStringUntil(StringView& outStrView, const char delimiter);
        bool ReadConditional(const StringView& expectedValue);

        template <typename T>
        bool ReadType(T& outValue)
        {
            ONYX_ASSERT(IsEof() == false);

            const char* startPos = (m_Data.data() + m_CurrentDataPosition);
            const std::from_chars_result result = std::from_chars(startPos, startPos + GetRemainingLength(), outValue);
            ONYX_ASSERT(result.ec != std::errc::invalid_argument);
            ONYX_ASSERT(result.ec != std::errc::result_out_of_range);

            onyxU32 count = static_cast<onyxU32>(result.ptr - startPos);
            Skip(count);
            return true;
        }

        template <typename T>
        void WriteType(const T& val)
        {
            String valStr = to_string(val);
            Write(valStr);
        }

    private:
        void DoRead(char* destination, onyxU64 size) const override;
        void DoWrite(const char* data, onyxU64 size) override;

    private:
        String m_Data;
        mutable onyxU64 m_CurrentDataPosition = 0;
    };
}
