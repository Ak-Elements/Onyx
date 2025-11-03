#include <onyx/filesystem/jsonserializer.h>

#include <nlohmann/json.hpp>

namespace Onyx::FileSystem
{
    JsonSerializer::JsonSerializer()
        : JsonRoot(nlohmann::ordered_json{})
    {
        JsonNodes.push(&JsonRoot);
    }

    template<typename T>
    bool JsonSerializer::DoGenericWrite(T value)
    {
        GetCurrent() = value;
        return true;
    }

    template<std::integral T>
    bool JsonSerializer::DoGenericWrite(T value, onyxU8 base)
    {
        StringView valueAsBaseString;
        switch (base)
        {
        case 2:
            valueAsBaseString = Format::Format("{:b}", value);
            break;
        case 8:
            valueAsBaseString = Format::Format("{:o}", value);
            break;
        case 10:
            valueAsBaseString = Format::Format("{}", value);
            break;
        case 16:
            valueAsBaseString = Format::Format("{:x}", value);
            break;
        default:
            ONYX_LOG_WARNING("Unsupported base for json {}, falling back to base 10.", base);
            valueAsBaseString = Format::Format("{}", value);
            break;
        }

        GetCurrent() = valueAsBaseString;
        return true;
    }

    template<typename T>
    bool JsonSerializer::DoGenericWrite(StringView name, T value)
    {
        GetCurrent()[name] = value;
        return true;
    }

    template<std::integral T>
    bool JsonSerializer::DoGenericWrite(StringView name, T value, onyxU8 base)
    {
        StringView valueAsBaseString;
        switch (base)
        {
        case 2:
            valueAsBaseString = Format::Format("{:b}", value);
            break;
        case 8:
            valueAsBaseString = Format::Format("{:o}", value);
            break;
        case 10:
            valueAsBaseString = Format::Format("{}", value);
            break;
        case 16:
            valueAsBaseString = Format::Format("{:x}", value);
            break;
        default:
            ONYX_LOG_WARNING("Unsupported base for json {}, falling back to base 10.", base);
            valueAsBaseString = Format::Format("{}", value);
            break;
        }

        GetCurrent()[name] = valueAsBaseString;
        return true;
    }

    bool JsonSerializer::DoWrite(bool value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(StringView name, bool value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(onyxS8 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxS16 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxS32 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxS64 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxU8 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxU16 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxU32 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxU64 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxF32 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxF64 value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(onyxS8 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxS16 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxS32 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxS64 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxU8 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxU16 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxU32 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(onyxU64 value, onyxU8 base)
    {
        return DoGenericWrite(value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS8 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS16 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS32 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS64 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU8 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU16 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU32 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU64 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxF32 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxF64 value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS8 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS16 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS32 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxS64 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU8 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU16 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU32 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView name, onyxU64 value, onyxU8 base)
    {
        return DoGenericWrite(name, value, base);
    }

    bool JsonSerializer::DoWrite(StringView value)
    {
        return DoGenericWrite(value);
    }

    bool JsonSerializer::DoWrite(StringView name, StringView value)
    {
        return DoGenericWrite(name, value);
    }

    bool JsonSerializer::CreateScope(onyxU32 index)
    {
        nlohmann::ordered_json& current = GetCurrent();
        if (current.is_object())
        {
            StringView scopeKey = Format::Format("{:x}", index);
            return CreateScope(scopeKey);
        }
        else
        {
            JsonNodes.push(&(GetCurrent()[index]));
            return true;
        }
    }

    bool JsonSerializer::CreateScope(onyxU64 index)
    {
        nlohmann::ordered_json& current = GetCurrent();
        if (current.is_object())
        {
            StringView scopeKey = Format::Format("{:x}", index);
            return CreateScope(scopeKey);
        }
        else
        {
            JsonNodes.push(&(GetCurrent()[index]));
            return true;
        }
    }

    bool JsonSerializer::CreateScope(StringView name)
    {
        nlohmann::ordered_json& current = GetCurrent();
        nlohmann::ordered_json& newCurrent = current[name];

        m_CurrentScopeName = name;
        JsonNodes.push(&newCurrent);
        return true;
    }

    bool JsonSerializer::EndScope()
    {
        JsonNodes.pop();
        return true;
    }

    nlohmann::ordered_json& JsonSerializer::GetCurrent()
    {
        return *JsonNodes.top();
    }

    nlohmann::ordered_json& JsonSerializer::GetCurrent() const
    {
        return *JsonNodes.top();
    }
}
