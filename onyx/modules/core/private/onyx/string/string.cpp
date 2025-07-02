#include <onyx/string/string.h>

namespace Onyx
{
    DynamicArray<String> Split(StringView string, StringView delimiters)
    {
		DynamicArray<String> result;

        if (string.empty())
            return result;

        const onyxU64 length = string.length();
        bool isEscaped = false;

        result.emplace_back();
        onyxU32 currentTokenIndex = 0;
        for (onyxU64 i = 0; i < length; ++i)
		{
            if (string[i] == '\\')
            {
                isEscaped = !isEscaped;
                continue;
            }

            if ((isEscaped == false) && delimiters.find(string[i]) != StringView::npos)
            {
                if (result[currentTokenIndex].empty() == false)
                {
                    result.emplace_back();
                    ++currentTokenIndex;
                }
            }
            else
            {
                isEscaped = false;
                result[currentTokenIndex] += string[i];
            }
		}

		return result;
    }

    DynamicArray<String> Split(StringView string, const char delimiter)
    {
		return Split(string, String(1, delimiter));
    }

    bool IgnoreCaseEqual(StringView lhs, StringView rhs)
    {
        return std::ranges::equal(lhs, rhs, [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); });
    }

    bool IgnoreCaseStartsWith(StringView string, StringView prefix)
    {
        auto it = std::ranges::mismatch(string.begin(), string.end(), prefix.begin(), prefix.end(), [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); });
        return it.in1 != string.end();
    }

    StringView::size_type IgnoreCaseFind(StringView string, StringView searchString)
    {
        auto it = std::ranges::search(string, searchString, [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); }).begin();
        if (it != string.end())
            return it - string.begin();
        else
            return StringView::npos;
    }

    // move to string header
    void ToLower(String& str)
    {
        std::ranges::transform(str, str.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }

    String ToLower(StringView str)
    {
        String result(str);
        ToLower(result);
        return result;
    }

    StringView Trim(StringView str)
    {
        if (str.empty())
            return str;

        StringView::size_type start = str.find_first_not_of("\n\r\t ");
        StringView::size_type end = str.find_last_not_of("\n\r\t ");

        if ((start == StringView::npos) && (end == StringView::npos))
        {
            return "";
        }

        return str.substr(start, (end - start) + 1);
    }

    StringView TrimLeft(StringView str)
    {
        if (str.empty())
            return str;

        StringView::size_type start = str.find_first_not_of("\n\r\t ");

        if (start == StringView::npos)
        {
            return str;
        }

        return str.substr(start);
    }
}
