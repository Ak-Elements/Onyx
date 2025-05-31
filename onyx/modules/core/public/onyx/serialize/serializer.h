#pragma once

#include <onyx/stringid.h>
#include <onyx/guid.h>
#include <onyx/serialize/serialization.h>
#include <onyx/geometry/vectorserialization.h>
#include <onyx/geometry/rectserialization.h>

namespace Onyx
{
    class Serializer
    {
        friend struct SerializationScope;
    public:
        Serializer() = default;
        virtual ~Serializer() = default;

        template <std::integral T>
        bool Write(const T& outValue, onyxU8 base)
        {
            return DoWrite(outValue, base);
        }

        template <CompileTimeString Name, std::integral T>
        bool Write(const T& outValue, onyxU8 base)
        {
            return DoWrite(Name.string_view(), outValue, base);
        }

        template <std::integral T>
        bool Write(StringView name, const T& outValue, onyxU8 base)
        {
            return DoWrite(name, outValue, base);
        }

        template <typename T>
        bool WriteAt(onyxU32 i, const T& outValue)
        {
            if (CreateScope(i) == false)
            {
                return false;
            }

            bool success = Write(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T>
        bool Write(const T& outValue)
        {
            if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, StringView>)
            {
                return DoWrite(outValue);
            }
            else if constexpr (std::is_same_v<T, String>)
            {
                return DoWrite(StringView(outValue));
            }
            else if constexpr (std::is_enum_v<T>)
            {
                bool success = DoWrite(Enums::ToString<T>(outValue));
                return success;
            }
            else
            {
                bool success = Serialization<T>::Serialize(*this, outValue);
                return success;
            }
        }

        template <typename T>
        bool Write(Guid64 id, const T& outValue)
        {
            if (CreateScope(id.Get()) == false)
            {
                return false;
            }

            if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, StringView>)
            {
                DoWrite(outValue);
                EndScope();
                return true;
            }
            else if constexpr (std::is_same_v<T, String>)
            {
                DoWrite(StringView(outValue));
                EndScope();
                return true;
            }
            else if constexpr (std::is_enum_v<T>)
            {
                bool success = DoWrite(Enums::ToString<T>(outValue));
                EndScope();
                return success;
            }
            else
            {
                bool success = Serialization<T>::Serialize(*this, outValue);
                EndScope();
                return success;
            }
        }

        template <CompileTimeString Name, typename T>
        bool Write(const T& outValue)
        {
            return Write(Name.string_view(), outValue);
        }

        template <typename T>
        bool Write(StringView name, const T& outValue)
        {
            if constexpr (std::is_fundamental_v<T> || std::is_same_v<T, StringView>)
            {
                return DoWrite(name, outValue);
            }
            else if constexpr (std::is_same_v<T, String>)
            {
                return DoWrite(name, StringView(outValue));
            }
            else if constexpr (std::is_enum_v<T>)
            {
                bool success = DoWrite(name, Enums::ToString<T>(outValue));
                return success;
            }
            else
            {
                if (CreateScope(name) == false)
                {
                    return false;
                }

                bool success = Write(outValue);
                success &= EndScope();
                return success;
            }
        }

        template <typename T>
        bool Write(const DynamicArray<T>& outValue)
        {
            bool success = true;
            onyxU32 count = numeric_cast<onyxU32>(outValue.size());
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }
                success = Write(outValue[i]);
                success &= EndScope();
                if (success == false)
                {
                    break;
                }
            }

            return success;
        }

        template <typename T>
        bool Write(StringView name, const DynamicArray<T>& outValue)
        {
            if (outValue.empty())
            {
                return true;
            }

            if (CreateScope(name) == false)
            {
                return false;
            }

            bool success = Write(outValue);
            success &= EndScope();
            return success;
        }

        template <typename T, onyxU8 Size>
        bool Write(StringView name, const InplaceArray<T, Size>& outValue)
        {
            if (outValue.empty())
            {
                return true;
            }

            if (CreateScope(name) == false)
            {
                return false;
            }

            bool success = true;
            onyxU8 count = outValue.size();
            for (onyxU8 i = 0; i < count; ++i)
            {
                success = CreateScope(static_cast<onyxU32>(i));
                if (success == false)
                {
                    break;
                }

                success &= Write(outValue[i]);
                success &= EndScope();

                if (success == false)
                {
                    break;
                }
            }

            success &= EndScope();
            return success;
        }

        template <typename KeyT, typename ValueT>
        bool Write(const HashMap<KeyT, ValueT>& map)
        {
            // write out count
            bool success = true;
            for (auto&& [key, value] : map)
            {
                if constexpr (std::is_integral_v<KeyT> || IsStringId<KeyT> || IsGuid64<KeyT>)
                {
                    if (IsSupportingIntegralScopes())
                    {
                        auto scopeKey = GetScopeKeyUnderlyingType(key);
                        success = CreateScope(scopeKey);
                        if (success == false)
                        {
                            break;
                        }

                        success &= Serializer::template Write<ValueT>(value);
                        success &= EndScope();

                        if (success == false)
                        {
                            break;
                        }

                        continue;
                    }
                }
                
                StringView keyStr = ScopeKeyToString(key);
                if (Serializer::template Write<ValueT>(keyStr, value) == false)
                {
                    return false;
                }
            }

            return success;
        }

        template <CompileTimeString Name, typename T, typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, const T&>
        bool WriteForEach(const DynamicArray<T>& value, Callable forEachFunctor)
        {
            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = WriteForEach(value, forEachFunctor);
            success &= EndScope();
            return success;
        }

        template <typename T, typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, const T&>
        bool WriteForEach(const DynamicArray<T>& value, Callable forEachFunctor)
        {
            bool success = true;
            onyxU32 count = numeric_cast<onyxU32>(value.size());
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                success = forEachFunctor(*this, value[i]);
                success &= EndScope();
                if (success == false)
                {
                    break;
                }
            }

            return success;
        }


        template <typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, onyxU32>
        bool WriteForEach(Callable forEachFunctor, onyxU32 count)
        {
            bool success = true;
            for (onyxU32 i = 0; i < count; ++i)
            {
                if (CreateScope(i) == false)
                {
                    return false;
                }

                success = forEachFunctor(*this, i);
                success &= EndScope();
                if (success == false)
                {
                    break;
                }
            }

            return success;
        }

        template <CompileTimeString Name, typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, onyxU32>
        bool WriteForEach(Callable forEachFunctor, onyxU32 count)
        {
            if (count == 0)
            {
                return true;
            }

            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = WriteForEach(forEachFunctor, count);
            success &= EndScope();
            return success;
        }

        template <CompileTimeString Name, typename KeyT, typename ValueT, typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, const KeyT&, const ValueT&>
        bool WriteForEach(const HashMap<KeyT, ValueT>& outMap, Callable forEachFunctor)
        {
            if (outMap.empty())
            {
                return true;
            }

            if (CreateScope(Name.string_view()) == false)
            {
                return false;
            }

            bool success = WriteForEach(outMap, forEachFunctor);
            success &= EndScope();
            return success;
        }

        template <typename KeyT, typename ValueT, typename Callable> requires std::is_invocable_r_v<bool, Callable, Serializer&, const KeyT&, const ValueT&>
        bool WriteForEach(const HashMap<KeyT, ValueT>& outMap, Callable forEachFunctor)
        {
            bool success = true;
            for (auto&& [key, value] : outMap)
            {
                if constexpr (std::is_integral_v<KeyT> || IsStringId<KeyT> || IsGuid64<KeyT>)
                {
                    if (IsSupportingIntegralScopes())
                    {
                        auto scopeKey = GetScopeKeyUnderlyingType(key);
                        success = CreateScope(scopeKey);
                        if (success == false)
                        {
                            break;
                        }

                        success &= forEachFunctor(*this, key, value);
                        success &= EndScope();

                        if (success == false)
                        {
                            break;
                        }

                        continue;
                    }
                }

                StringView keyStr = ScopeKeyToString(key);
                success = CreateScope(keyStr);
                if (success == false)
                {
                    break;
                }

                success &= forEachFunctor(*this, key, value);
                success &= EndScope();
                if (success == false)
                {
                    break;
                }
            }

            return success;
        }

        template <CompileTimeString Name>
        SerializationScope EnterScope() { return SerializationScope(*this, Name.string_view()); }
        SerializationScope EnterScope(onyxU32 i) { return SerializationScope(*this, i); }

    private:
        virtual bool DoWrite(bool value) = 0;
        virtual bool DoWrite(StringView name, bool value) = 0;

        virtual bool DoWrite(onyxS8 value) = 0;
        virtual bool DoWrite(onyxS16 value) = 0;
        virtual bool DoWrite(onyxS32 value) = 0;
        virtual bool DoWrite(onyxS64 value) = 0;
        virtual bool DoWrite(onyxU8 value) = 0;
        virtual bool DoWrite(onyxU16 value) = 0;
        virtual bool DoWrite(onyxU32 value) = 0;
        virtual bool DoWrite(onyxU64 value) = 0;
        virtual bool DoWrite(onyxS8 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxS16 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxS32 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxS64 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxU8 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxU16 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxU32 value, onyxU8 base) = 0;
        virtual bool DoWrite(onyxU64 value, onyxU8 base) = 0;

        virtual bool DoWrite(StringView name, onyxS8 value) = 0;
        virtual bool DoWrite(StringView name, onyxS16 value) = 0;
        virtual bool DoWrite(StringView name, onyxS32 value) = 0;
        virtual bool DoWrite(StringView name, onyxS64 value) = 0;
        virtual bool DoWrite(StringView name, onyxU8 value) = 0;
        virtual bool DoWrite(StringView name, onyxU16 value) = 0;
        virtual bool DoWrite(StringView name, onyxU32 value) = 0;
        virtual bool DoWrite(StringView name, onyxU64 value) = 0;
        virtual bool DoWrite(StringView name, onyxS8 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxS16 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxS32 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxS64 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxU8 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxU16 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxU32 value, onyxU8 base) = 0;
        virtual bool DoWrite(StringView name, onyxU64 value, onyxU8 base) = 0;

        virtual bool DoWrite(onyxF32 value) = 0;
        virtual bool DoWrite(onyxF64 value) = 0;
        virtual bool DoWrite(StringView name, onyxF32 value) = 0;
        virtual bool DoWrite(StringView name, onyxF64 value) = 0;

        virtual bool DoWrite(StringView value) = 0;
        virtual bool DoWrite(StringView name, StringView value) = 0;

        virtual bool CreateScope(onyxU32 index) = 0;
        virtual bool CreateScope(onyxU64 index) = 0;
        virtual bool CreateScope(StringView name) = 0;

        virtual bool EndScope() = 0;

        virtual onyxU32 GetItemsCount() = 0;

        virtual bool GetScopeIdentifier(onyxU32& outKey) = 0;
        virtual bool GetScopeIdentifier(onyxU64& outKey) = 0;
        virtual bool GetScopeIdentifier(Guid64& outKey) = 0;
        virtual bool GetScopeIdentifier(StringView& outKey) = 0;


        virtual bool IsSupportingIntegralScopes() const = 0;

        template <typename T>
        auto GetScopeKeyUnderlyingType(T key)
        {
            if constexpr (IsStringId<T>)
            {
                return key.GetId();
            }
            else if constexpr (IsGuid64<T>)
            {
                return key.Get();
            }
            else
            {
                return key;
            }
        }

        template <typename T>
        StringView ScopeKeyToString(T key)
        {
            if constexpr (IsStringId<T>)
            {
                return Format::Format("{:x}", key.GetId());
            }
            else if constexpr (IsGuid64<T>)
            {
                return Format::Format("{:x}", key.Get());
            }
            else
            {
                return Format::Format("{}", key);
            }
        }
    };

}