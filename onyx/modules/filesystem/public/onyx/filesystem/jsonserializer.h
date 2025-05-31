#pragma once

#include <onyx/serialize/serializer.h>
#include <nlohmann/json.hpp>

namespace Onyx::FileSystem
{
    class JsonSerializer : public Serializer
    {
    public:
        JsonSerializer();

    private:
        template <typename T>
        bool DoGenericWrite(T outValue);

        template <std::integral T>
        bool DoGenericWrite(T outValue, onyxU8 base);

        template <typename T>
        bool DoGenericWrite(StringView name, T outValue);

        template <std::integral T>
        bool DoGenericWrite(StringView name, T outValue, onyxU8 base);

        nlohmann::ordered_json& GetCurrent();
        nlohmann::ordered_json& GetCurrent() const;

    private:
        bool DoWrite(bool value) override;
        bool DoWrite(StringView name, bool value) override;

        bool DoWrite(onyxS8 value) override;
        bool DoWrite(onyxS16 value) override;
        bool DoWrite(onyxS32 value) override;
        bool DoWrite(onyxS64 value) override;
        bool DoWrite(onyxU8 value) override;
        bool DoWrite(onyxU16 value) override;
        bool DoWrite(onyxU32 value) override;
        bool DoWrite(onyxU64 value) override;
        bool DoWrite(onyxS8 value, onyxU8 base) override;
        bool DoWrite(onyxS16 value, onyxU8 base) override;
        bool DoWrite(onyxS32 value, onyxU8 base) override;
        bool DoWrite(onyxS64 value, onyxU8 base) override;
        bool DoWrite(onyxU8 value, onyxU8 base) override;
        bool DoWrite(onyxU16 value, onyxU8 base) override;
        bool DoWrite(onyxU32 value, onyxU8 base) override;
        bool DoWrite(onyxU64 value, onyxU8 base) override;

        bool DoWrite(StringView name, onyxS8 value) override;
        bool DoWrite(StringView name, onyxS16 value) override;
        bool DoWrite(StringView name, onyxS32 value) override;
        bool DoWrite(StringView name, onyxS64 value) override;
        bool DoWrite(StringView name, onyxU8 value) override;
        bool DoWrite(StringView name, onyxU16 value) override;
        bool DoWrite(StringView name, onyxU32 value) override;
        bool DoWrite(StringView name, onyxU64 value) override;
        bool DoWrite(StringView name, onyxS8 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxS16 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxS32 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxS64 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxU8 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxU16 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxU32 value, onyxU8 base) override;
        bool DoWrite(StringView name, onyxU64 value, onyxU8 base) override;

        bool DoWrite(onyxF32 value) override;
        bool DoWrite(onyxF64 value) override;
        bool DoWrite(StringView name, onyxF32 value) override;
        bool DoWrite(StringView name, onyxF64 value) override;

        bool DoWrite(StringView value) override;
        bool DoWrite(StringView name, StringView value) override;

        bool CreateScope(onyxU32 index) override;

        bool CreateScope(onyxU64 index) override;

        bool CreateScope(StringView name) override;

        bool EndScope() override;

        onyxU32 GetItemsCount() override;

        bool GetScopeIdentifier(onyxU32& outKey) override;
        bool GetScopeIdentifier(onyxU64& outKey) override;
        bool GetScopeIdentifier(Guid64& outKey) override;
        bool GetScopeIdentifier(StringView& outKey) override;

        bool IsSupportingIntegralScopes() const override { return false; }

    public:
        nlohmann::ordered_json JsonRoot;

    private:
        mutable Stack<nlohmann::ordered_json*> JsonNodes;
        mutable StringView m_CurrentScopeName;
    };
}
