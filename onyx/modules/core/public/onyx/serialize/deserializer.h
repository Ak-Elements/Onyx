#pragma once

#include <onyx/stringid.h>
#include <onyx/guid.h>
#include <onyx/serialize/serialization.h>
#include <onyx/geometry/vectorserialization.h>
#include <onyx/geometry/rectserialization.h>

namespace Onyx
{
    class Deserializer
    {
    public:
        Deserializer() = default;
        virtual ~Deserializer() = default;

        template <CompileTimeString Name, typename T>
        bool Read(T& outValue) const
        {
            return Read(Name.string_view(), outValue);
        }

        template <CompileTimeString Name, typename T>
        bool ReadOptional(T& outValue) const
        {
            if (CreateScope(Name.string_view()) == false)
            {
                return true;
            }

            bool success = Read(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T>
        bool ReadAt(onyxU32 i, T& outValue) const
        {
            if (CreateScope(i) == false)
            {
                return false;
            }

            bool success = Read(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T>
        bool Read(T& outValue) const
        {
            if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, StringView>)
            {
                return DoRead(outValue);
            }
            else if constexpr (std::is_same_v<T, String>)
            {
                StringView stringView;
                bool success = DoRead(stringView);
                outValue = stringView;
                return success;
            }
            else if constexpr (std::is_enum_v<T>)
            {
                StringView enumValue;
                if (DoRead(enumValue))
                {
                    outValue = Enums::FromString<T>(enumValue);
                    return true;
                }
                return false;
            }
            else
            {
                bool success = Serialization<T>::Deserialize(*this, outValue);
                return success;
            }
        }

        template <typename T>
        bool Read(StringView name, T& outValue) const
        {
            if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, StringView>)
            {
                return DoRead(name, outValue);
            }
            else if constexpr (std::is_same_v<T, String>)
            {
                StringView stringView;
                bool success = DoRead(name, stringView);
                outValue = stringView;
                return success;
            }
            else if constexpr (std::is_enum_v<T>)
            {
                StringView enumValue;
                if (DoRead(name, enumValue))
                {
                    outValue = Enums::FromString<T>(enumValue);
                    return true;
                }

                return false;
            }
            else
            {
                if (CreateScope(name) == false)
                {
                    return false;
                }

                bool success = Read(outValue);
                success &= EndScope();

                return success;
            }
        }

        template <std::integral T>
        bool Read(T& outValue, onyxU8 base) const
        {
            return DoRead(outValue, base);
        }
        template <CompileTimeString Name, std::integral T>
        bool Read(T& outValue, onyxU8 base) const
        {
            return DoRead(Name.string_view(), outValue, base);
        }

        template <typename T>
        bool Read(DynamicArray<T>& outValue) const
        {
            bool success = false;
            onyxU32 count = GetItemsCount();
            outValue.reserve(count);
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                T& arrayValue = outValue.emplace_back();
                success = Read(arrayValue);
                success &= EndScope();
            }
            
            return success;
        }

        template <typename T, typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&, T&>
        bool ReadForEach(DynamicArray<T>& outValue, Callable forEachFunctor) const
        {
            bool success = false;
            onyxU32 count = GetItemsCount();
            outValue.reserve(count);
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                T& arrayValue = outValue.emplace_back();
                success = forEachFunctor(*this, arrayValue);
                success &= EndScope();
            }

            return success;
        }

        template <CompileTimeString Name, typename T, typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&, T&>
        bool ReadForEach(DynamicArray<T>& outValue, Callable forEachFunctor) const
        {
            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = false;
            onyxU32 count = GetItemsCount();
            outValue.reserve(count);
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                T& arrayValue = outValue.emplace_back();
                success = forEachFunctor(*this, arrayValue);
                success &= EndScope();
            }

            success &= EndScope();
            return success;
        }

        template <typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&>
        bool ReadForEach(Callable forEachFunctor) const
        {
            bool success = false;
            onyxU32 count = GetItemsCount();
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                success = forEachFunctor(*this);
                success &= EndScope();
            }

            return success;
        }

        template <CompileTimeString Name, typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&>
        bool ReadForEach(Callable forEachFunctor) const
        {
            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = ReadForEach(forEachFunctor);
            success &= EndScope();
            return success;
        }

        template <typename KeyT, typename ValueT, typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&, const KeyT&, ValueT&>
        bool ReadForEach(HashMap<KeyT, ValueT>& outMap, Callable forEachFunctor) const
        {
            bool success = false;
            onyxU32 count = GetItemsCount();
            for (onyxU32 i = 0; i < count; ++i)
            {
                ValueT mapValue;
                if constexpr (std::is_integral_v<KeyT> || IsStringId<KeyT> || IsGuid64<KeyT>)
                {
                    if (IsSupportingIntegralScopes())
                    {
                        if (CreateScope(i) == false)
                        {
                            return false;
                        }

                        auto scopeKey = GetScopeKeyUnderlyingType<KeyT>();
                        GetScopeIdentifier(scopeKey);
                        KeyT mapKey(scopeKey);

                        success = forEachFunctor(*this, mapKey, mapValue);
                        success &= EndScope();

                        if (success == false)
                        {
                            return false;
                        }

                        outMap[mapKey] = std::move(mapValue);
                        continue;
                    }
                }

                if (CreateScope(i) == false)
                {
                    return false;
                }

                // convert from stringview to key type
                StringView keyString;
                GetScopeIdentifier(keyString);
                KeyT mapKey = StringToScopeKey<KeyT>(keyString);

                success = forEachFunctor(*this, mapKey, mapValue);
                success &= EndScope();
                if (success == false)
                {
                    return false;
                }

                outMap[mapKey] = std::move(mapValue);
            }

            return success;
        }

        template <CompileTimeString Name, typename KeyT, typename ValueT, typename Callable> requires std::is_invocable_r_v<bool, Callable, const Deserializer&, const KeyT&, ValueT&>
        bool ReadForEach(HashMap<KeyT, ValueT>& outMap, Callable forEachFunctor) const
        {
            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = ReadForEach(outMap, forEachFunctor);
            success &= EndScope();
            return success;
        }

        template <typename T>
        bool Read(StringView name, DynamicArray<T>& outValue) const
        {
            if (CreateScope(name) == false)
            {
                return false;
            }

            bool success = Read(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T, onyxU8 Size>
        bool Read(StringView name, InplaceArray<T, Size>& outValue) const
        {
            if (CreateScope(name) == false)
            {
                return false;
            }

            bool success = Read(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T, onyxU8 Size>
        bool Read(InplaceArray<T, Size>& outValue) const
        {
            bool success = true;
            onyxU8 count = numeric_cast<onyxU8>(GetItemsCount());
            for (onyxU8 i = 0; i < count; ++i)
            {
                T& arrayValue = outValue.Emplace();

                success &= CreateScope(static_cast<onyxU32>(i));
                if (success == false)
                {
                    break;
                }

                success = Read(arrayValue);
                success &= EndScope();

                if (success == false)
                {
                    break;
                }
            }

            return success;
        }

        template <typename KeyT, typename ValueT, typename... Args>
        bool Read(HashMap<KeyT, ValueT>& out, Args&&... additionalArgs) const
        {
            bool success = false;
            onyxU32 count = GetItemsCount();
            for (onyxU32 i = 0; i < count; ++i)
            {
                ValueT mapValue;
                if constexpr (std::is_integral_v<KeyT> || IsStringId<KeyT> || IsGuid64<KeyT>)
                {
                    if (IsSupportingIntegralScopes())
                    {
                        if (CreateScope(i) == false)
                        {
                            return false;
                        }

                        auto scopeKey = GetScopeKeyUnderlyingType<KeyT>();

                        GetScopeIdentifier(scopeKey);
                        success = Deserializer::template Read<ValueT, Args...>(mapValue, std::forward<Args...>(additionalArgs)...);
                        success &= EndScope();

                        if (success == false)
                        {
                            return false;
                        }

                        out[KeyT(scopeKey)] = std::move(mapValue);
                        continue;
                    }
                }

                if (CreateScope(i) == false)
                {
                    return false;
                }

                StringView keyString;
                GetScopeIdentifier(keyString);
                KeyT mapKey = StringToScopeKey<KeyT>(keyString);

                success = Deserializer::template Read<ValueT, Args...>(mapValue, std::forward<Args...>(additionalArgs)...);
                success &= EndScope();

                if (success == false)
                {
                    return false;
                }

                out[mapKey] = std::move(mapValue);
            }

            return success;
        }

    private:
        virtual bool DoRead(bool& outValue) const = 0;
        virtual bool DoRead(StringView name, bool& outValue) const = 0;

        virtual bool DoRead(onyxS8& outValue) const = 0;
        virtual bool DoRead(onyxS16& outValue) const = 0;
        virtual bool DoRead(onyxS32& outValue) const = 0;
        virtual bool DoRead(onyxS64& outValue) const = 0;
        virtual bool DoRead(onyxU8& outValue) const = 0;
        virtual bool DoRead(onyxU16& outValue) const = 0;
        virtual bool DoRead(onyxU32& outValue) const = 0;
        virtual bool DoRead(onyxU64& outValue) const = 0;
        virtual bool DoRead(onyxS8& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxS16& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxS32& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxS64& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxU8& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxU16& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxU32& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(onyxU64& outValue, onyxU8 base) const = 0;

        virtual bool DoRead(StringView name, onyxS8& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxS16& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxS32& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxS64& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxU8& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxU16& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxU32& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxU64& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxS8& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxS16& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxS32& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxS64& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxU8& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxU16& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxU32& outValue, onyxU8 base) const = 0;
        virtual bool DoRead(StringView name, onyxU64& outValue, onyxU8 base) const = 0;

        virtual bool DoRead(onyxF32& outValue) const = 0;
        virtual bool DoRead(onyxF64& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxF32& outValue) const = 0;
        virtual bool DoRead(StringView name, onyxF64& outValue) const = 0;

        virtual bool DoRead(StringView& outValue) const = 0;
        virtual bool DoRead(StringView name, StringView& outValue) const = 0;

        virtual bool CreateScope(onyxU32 index) const = 0;
        virtual bool CreateScope(onyxU64 index) const = 0;
        virtual bool CreateScope(StringView name) const = 0;

        virtual bool EndScope() const = 0;

        virtual onyxU32 GetItemsCount() const = 0;

        virtual bool GetScopeIdentifier(onyxU32& outKey) const = 0;
        virtual bool GetScopeIdentifier(onyxU64& outKey) const = 0;
        virtual bool GetScopeIdentifier(Guid64& outKey) const = 0;
        virtual bool GetScopeIdentifier(StringView& outKey) const = 0;

        virtual bool IsSupportingIntegralScopes() const = 0;

        template <typename T>
        auto GetScopeKeyUnderlyingType() const -> decltype(auto)
        {
            if constexpr (IsStringId<T>)
            {
                return T::Invalid;
            }
            else if constexpr (IsGuid64<T>)
            {
                return onyxU64{ 0 };
            }
            else if constexpr (std::is_integral_v<T>)
            {
                return T{ 0 };
            }
            else
            {
                return StringView{};
            }
        }

        template <typename T>
        auto StringToScopeKey(StringView keyString) const
        {
            if constexpr (std::is_integral_v<T> || IsStringId<T> || IsGuid64<T>)
            {
                if constexpr (IsStringId<T>)
                {
                    typename T::IdType id = 0;
                    std::ignore = std::from_chars(keyString.data(), keyString.data() + keyString.size(), id, 16);
                    return T{ id };
                }
                else if constexpr (IsGuid64<T>)
                {
                    onyxU64 id = 0;
                    std::ignore = std::from_chars(keyString.data(), keyString.data() + keyString.size(), id, 16);
                    return T{ id };
                }
                else
                {
                    T key{};
                    std::ignore = std::from_chars(keyString.data(), keyString.data() + keyString.size(), key, 10);
                    return key;
                }
            }
            else
            {
                return keyString;
            }
        }
    };
}