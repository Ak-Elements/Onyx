#pragma once

#include <nlohmann/json.hpp>
#include <onyx/serialize/deserializer.h>
#include <nlohmann/json_fwd.hpp>

namespace Onyx::FileSystem
{
    class JsonDeserializer : public Deserializer
    {
    public:
        JsonDeserializer();
        JsonDeserializer(const nlohmann::ordered_json& json);

    private:
        template <typename T>
        bool DoGenericRead(T& outValue) const;

        template <std::integral T>
        bool DoGenericRead(T& outValue, onyxU8 base) const;

        template <typename T>
        bool DoGenericRead(StringView name, T& outValue) const;

        template <std::integral T>
        bool DoGenericRead(StringView name, T& outValue, onyxU8 base) const;

        nlohmann::ordered_json& GetCurrent();
        nlohmann::ordered_json& GetCurrent() const;

    private:
        // Serializer interface
        bool DoRead(bool& outValue) const override;
        bool DoRead(StringView name, bool& outValue) const override;

        bool DoRead(onyxS8& outValue) const override;
        bool DoRead(onyxS16& outValue) const override;
        bool DoRead(onyxS32& outValue) const override;
        bool DoRead(onyxS64& outValue) const override;
        bool DoRead(onyxU8& outValue) const override;
        bool DoRead(onyxU16& outValue) const override;
        bool DoRead(onyxU32& outValue) const override;
        bool DoRead(onyxU64& outValue) const override;
        bool DoRead(onyxS8& outValue, onyxU8 base) const override;
        bool DoRead(onyxS16& outValue, onyxU8 base) const override;
        bool DoRead(onyxS32& outValue, onyxU8 base) const override;
        bool DoRead(onyxS64& outValue, onyxU8 base) const override;
        bool DoRead(onyxU8& outValue, onyxU8 base) const override;
        bool DoRead(onyxU16& outValue, onyxU8 base) const override;
        bool DoRead(onyxU32& outValue, onyxU8 base) const override;
        bool DoRead(onyxU64& outValue, onyxU8 base) const override;

        bool DoRead(StringView name, onyxS8& outValue) const override;
        bool DoRead(StringView name, onyxS16& outValue) const override;
        bool DoRead(StringView name, onyxS32& outValue) const override;
        bool DoRead(StringView name, onyxS64& outValue) const override;
        bool DoRead(StringView name, onyxU8& outValue) const override;
        bool DoRead(StringView name, onyxU16& outValue) const override;
        bool DoRead(StringView name, onyxU32& outValue) const override;
        bool DoRead(StringView name, onyxU64& outValue) const override;
        bool DoRead(StringView name, onyxS8& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxS16& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxS32& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxS64& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxU8& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxU16& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxU32& outValue, onyxU8 base) const override;
        bool DoRead(StringView name, onyxU64& outValue, onyxU8 base) const override;

        bool DoRead(onyxF32& outValue) const override;
        bool DoRead(onyxF64& outValue) const override;
        bool DoRead(StringView name, onyxF32& outValue) const override;
        bool DoRead(StringView name, onyxF64& outValue) const override;

        bool DoRead(StringView& outValue) const override;
        bool DoRead(StringView name, StringView& outValue) const override;

        bool CreateScope(onyxU32 index) const override;
        bool CreateScope(onyxU64 index) const override;
        bool CreateScope(StringView name) const override;
        bool EndScope() const override;

        onyxU32 GetItemsCount() const override;

        bool GetScopeIdentifier(onyxU32& outKey) const override;
        bool GetScopeIdentifier(onyxU64& outKey) const override;
        bool GetScopeIdentifier(Guid64& outKey) const override;
        bool GetScopeIdentifier(StringView& outKey) const override;

        bool IsSupportingIntegralScopes() const override { return false; }

    public:
        nlohmann::ordered_json JsonRoot;

    private:
        mutable Stack<nlohmann::ordered_json*> JsonNodes;
        mutable StringView m_CurrentScopeName;
    };
}
