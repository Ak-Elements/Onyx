#include <onyx/stream/stream.h>

namespace Onyx
{
    void Stream::Read(String& outStr) const
    {
        onyxU64 length = 0;
        Read(length);
        Read(outStr, length);
    }

    void Stream::Read(String& outStr, onyxU64 length) const
    {
        outStr.resize(length);
        DoRead(outStr.data(), length);
    }

    /*void Stream::Read(StringView& outStr, onyxU64 length) const
    {
        outStr = { "", static_cast<StringView::size_type>(length) };
        DoRead(outStr.data(), length);
    }*/

    void Stream::Write(const String& val)
    {
        Write(static_cast<onyxU64>(val.size()));
        DoWrite(val.data(), sizeof(char) * val.size());
    }

    void Stream::Write(const StringView& val)
    {
        Write(static_cast<onyxU64>(val.size()));
        DoWrite(val.data(), sizeof(char) * val.size());
    }
}
