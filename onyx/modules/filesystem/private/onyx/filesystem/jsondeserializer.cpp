#include <onyx/filesystem/jsondeserializer.h>

#include <nlohmann/json.hpp>

namespace onyx::file_system
{
    JsonDeserializer::JsonDeserializer(const nlohmann::ordered_json& json)
        : JsonRoot(json)
    {
        JsonNodes.emplace(&JsonRoot);
    }

    template<typename T>
    bool JsonDeserializer::DoGenericRead(T& outValue) const
    {
        outValue = GetCurrent().get<T>();
        return true;
    }

    template<std::integral T>
    bool JsonDeserializer::DoGenericRead(T& outValue, onyxU8 base) const
    {
        StringView valueAsBaseString = GetCurrent().get<StringView>();
        bool success = std::from_chars(valueAsBaseString.data(), valueAsBaseString.data() + valueAsBaseString.size(), outValue, base).ec == std::errc{};
        return success;
    }

    template<typename T>
    bool JsonDeserializer::DoGenericRead(StringView name, T& outValue) const
    {
        auto it = GetCurrent().find(name);
        if (it == GetCurrent().end())
        {
            return false;
        }

        outValue = it->get<T>();
        return true;
    }

    template<std::integral T>
    bool JsonDeserializer::DoGenericRead(StringView name, T& outValue, onyxU8 base) const
    {
        StringView valueAsBaseString = GetCurrent()[name].get<StringView>();
        bool success = std::from_chars(valueAsBaseString.data(), valueAsBaseString.data() + valueAsBaseString.size(), outValue, base).ec == std::errc{};
        return success;
    }

    bool JsonDeserializer::DoRead(bool& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, bool& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(onyxS8& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxS16& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxS32& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxS64& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxU8& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxU16& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxU32& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxU64& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxF32& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxF64& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(onyxS8& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxS16& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxS32& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxS64& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxU8& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxU16& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxU32& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(onyxU64& outValue, onyxU8 base) const
    {
        return DoGenericRead(outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS8& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS16& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS32& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS64& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU8& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU16& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU32& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU64& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxF32& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxF64& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS8& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS16& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS32& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxS64& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU8& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU16& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU32& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView name, onyxU64& outValue, onyxU8 base) const
    {
        return DoGenericRead(name, outValue, base);
    }

    bool JsonDeserializer::DoRead(StringView& outValue) const
    {
        return DoGenericRead(outValue);
    }

    bool JsonDeserializer::DoRead(StringView name, StringView& outValue) const
    {
        return DoGenericRead(name, outValue);
    }

    bool JsonDeserializer::CreateScope(onyxU32 index) const
    {
        nlohmann::ordered_json& current = GetCurrent();
        if (current.is_object())
        {
            onyxU32 currentIndex = 0;
            for (const auto& item : current.items())
            {
                if (currentIndex == index)
                {
                    m_CurrentScopeName = item.key();
                    JsonNodes.push(&(GetCurrent()[item.key()]));
                    return true;
                }
                ++currentIndex;
            }

            return false;
        }
        else
        {
            JsonNodes.push(&(GetCurrent()[index]));
            return true;
        }
    }

    bool JsonDeserializer::CreateScope(onyxU64 index) const
    {
        nlohmann::ordered_json& current = GetCurrent();
        if (current.is_object())
        {
            onyxU32 currentIndex = 0;
            for (const auto& item : current.items())
            {
                if (currentIndex == index)
                {
                    m_CurrentScopeName = item.key();
                    JsonNodes.push(&(GetCurrent()[item.key()]));
                    return true;
                }
            }

            return false;
        }
        else
        {
            JsonNodes.push(&(GetCurrent()[index]));
            return true;
        }
    }

    bool JsonDeserializer::CreateScope(StringView name) const
    {
        nlohmann::ordered_json& current = GetCurrent();
        if (current.is_object() == false)
            return false;

        for (auto it = current.begin(); it != current.end(); ++it)
        {
            if (IgnoreCaseEqual(it.key(), name))
            {
                m_CurrentScopeName = name;
                nlohmann::ordered_json& newScope = *it;
                JsonNodes.push(&newScope);
                return true;
            }
        }

        return false;
    }

    bool JsonDeserializer::GetScopeIdentifier(onyxU32& /*outKey*/) const
    {
        ONYX_ASSERT(false, "Integral scope identifiers not supported in json");
        return false;
    }

    bool JsonDeserializer::GetScopeIdentifier(onyxU64& /*outKey*/) const
    {
        ONYX_ASSERT(false, "Integral scope identifiers not supported in json");
        return false;
    }

    bool JsonDeserializer::GetScopeIdentifier(Guid64& outKey) const
    {
        onyxU64 guid64 = 0;
        bool success = std::from_chars(m_CurrentScopeName.data(), m_CurrentScopeName.data() + m_CurrentScopeName.size(), guid64, 16).ec == std::errc{};
        outKey = Guid64(guid64);
        return success;
    }

    bool JsonDeserializer::GetScopeIdentifier(StringView& outKey) const
    {
        outKey = m_CurrentScopeName;
        return true;
    }

    bool JsonDeserializer::EndScope() const
    {
        JsonNodes.pop();
        return true;
    }

    onyxU32 JsonDeserializer::GetItemsCount() const
    {
        return numeric_cast<onyxU32>(GetCurrent().size());
    }

    nlohmann::ordered_json& JsonDeserializer::GetCurrent()
    {
        return *JsonNodes.top();
    }

    nlohmann::ordered_json& JsonDeserializer::GetCurrent() const
    {
        return *JsonNodes.top();
    }
}
