#include <onyx/stream/stringstream.h>

namespace Onyx
{
    StringStream::StringStream(const String& str)
        : m_Data(str)
    {
    }

    bool StringStream::ReadConditional(const StringView& expectedValue)
    {
        ONYX_ASSERT(IsEof() == false);

        const onyxU32 count = static_cast<onyxU32>(expectedValue.size());
        if (GetRemainingLength() < count)
            return false;

        if (IgnoreCaseEqual(expectedValue.data(), StringView(m_Data.data() + m_CurrentDataPosition, count)) == false)
            return false;

        m_CurrentDataPosition += count;
        return true;
    }

    void StringStream::SkipWhitespaces()
    {
        const char* startPos = m_Data.data() + m_CurrentDataPosition;
        const char* endPos = startPos + GetRemainingLength();
        const char* foundPos = std::find_if(startPos, endPos, [](const char c) { return std::isspace(c) == 0; });

        onyxU32 count = static_cast<onyxU32>(foundPos - startPos);
        Skip(count);
    }

    bool StringStream::ReadLine(StringView& outStrView)
    {
        ONYX_ASSERT(IsEof() == false);

        const char* startPos = m_Data.data() + m_CurrentDataPosition;
        const char* endPos = startPos + GetRemainingLength();
        const char* foundPos = std::find(startPos, endPos, '\n') + 1; // include new line character

        onyxU32 count = static_cast<onyxU32>(std::min(static_cast<onyxU64>(foundPos - startPos), GetRemainingLength()));
        outStrView = { startPos, count };
        Skip(count);
        return true;
    }

    bool StringStream::ReadString(StringView& outStrView, const char delimiter)
    {
        const char* startPos = m_Data.data() + m_CurrentDataPosition;
        const char* endPos = startPos + GetRemainingLength();
        const char* strStartPos = std::find(startPos, endPos, delimiter);
        const char* strEndPos = std::find(strStartPos + 1, endPos,delimiter);

        if (strEndPos >= endPos)
            return false;

        onyxU32 count = static_cast<onyxU32>(strEndPos - startPos);
        outStrView = { strStartPos + 1, count - 2 }; // exclude " from string
        Skip(count); // + 1 to skip end delimiter
        return true;
    }

    bool StringStream::ReadStringUntil(StringView& outStrView, const char delimiter)
    {
        const char* startPos = m_Data.data() + m_CurrentDataPosition;
        const char* endPos = startPos + GetRemainingLength();
        const char* strEndPos = std::find(startPos, endPos, delimiter);

        if (strEndPos >= endPos)
            return false;

        onyxU32 count = static_cast<onyxU32>(strEndPos - startPos);
        outStrView = { startPos, count };
        Skip(count);
        return true;
    }

    bool StringStream::ReadString(StringView& outStrView)
    {
        return ReadString(outStrView, '"');
    }

    void StringStream::DoRead(char* destination, onyxU64 size) const
    {
        ONYX_UNUSED(destination);
        ONYX_ASSERT((m_CurrentDataPosition + size) < GetLength());
        char* startPos = const_cast<char*>(m_Data.data());
        destination = startPos + m_CurrentDataPosition;
        m_CurrentDataPosition += size;
    }

    void StringStream::DoWrite(const char* data, onyxU64 size)
    {
        m_Data.append(data, size);
        m_CurrentDataPosition += size;
    }
}
