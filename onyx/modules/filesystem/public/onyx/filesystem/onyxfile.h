#pragma once

#include <onyx/filesystem/filestream.h>
#include <filesystem>

#include <nlohmann/json.hpp>
#include <onyx/string/format.h>

namespace Onyx::FileSystem
{
    struct JsonValue
    {
        template <typename ValueT>
        bool Get(const StringView& name, ValueT& outValue) const
        {
            if (const auto& it = Json.find(name.data()); it != Json.end())
            {
                if constexpr (std::is_enum_v<ValueT>)
                {
                    outValue = Enums::FromString<ValueT>(it->get<StringView>());
                }
                else if constexpr (std::is_same_v<ValueT, Guid64>)
                {
                    outValue = Guid64(std::stoull(it->get<String>(), nullptr, 16));
                }
                else if constexpr (is_specialization_of_v<Vector2, ValueT>)
                {
                    std::array<typename ValueT::ScalarT, 2> data;
                    it->get_to(data);
                    outValue[0] = data[0];
                    outValue[1] = data[1];
                }
                else if constexpr (is_specialization_of_v<Vector3, ValueT>)
                {
                    std::array<typename ValueT::ScalarT, 3> data;
                    it->get_to(data);
                    outValue[0] = data[0];
                    outValue[1] = data[1];
                    outValue[2] = data[2];
                }
                else if constexpr (is_specialization_of_v<Vector4, ValueT>)
                {
                    std::array<typename ValueT::ScalarT, 4> data;
                    it->get_to(data);
                    outValue[0] = data[0];
                    outValue[1] = data[1];
                    outValue[2] = data[2];
                    outValue[3] = data[3];
                }
                else
                    outValue = it->get<ValueT>();
                return true;
            }

            return false;
        }

        template <typename T>
        bool Get(const StringView& name, T& outValue, T defaultValue) const
        {
            if (const auto& it = Json.find(name.data()); it != Json.end())
            {
                if constexpr (std::is_enum_v<T>)
                    outValue = Enums::FromString<T>(it->get<StringView>(), defaultValue);
                else
                    outValue = it->get<T>();
                return true;
            }

            outValue = defaultValue;
            return false;
        }

        void Add(const JsonValue& json)
        {
            return Json.push_back(json.Json);
        }

        template <typename KeyT, typename ValueT>
        void Set(const KeyT& key, const ValueT& value)
        {
            if constexpr (std::is_enum_v<ValueT>)
            {
                Json[key] = Enums::ToString(value);
            }
            else if constexpr (std::is_same_v<ValueT, JsonValue>)
            {
                Json[key] = std::move(value.Json);
            }
            else if constexpr (std::is_same_v<ValueT, Guid64>)
            {
                Json[key] = Format::Format("0x{:x}", value.Get());
            }
            else if constexpr (is_specialization_of_v<Vector2, ValueT>)
            {
                std::array<typename ValueT::ScalarT, 2> data{ value[0], value[1] };
                Json[key] = data;
            }
            else if constexpr (is_specialization_of_v<Vector3, ValueT>)
            {
                std::array<typename ValueT::ScalarT, 3> data{ value[0], value[1], value[2] };
                Json[key] = data;
            }
            else if constexpr (is_specialization_of_v<Vector4, ValueT>)
            {
                std::array<typename ValueT::ScalarT, 4> data{ value[0], value[1], value[2], value[3] };
                Json[key] = data;
            }
            else
            {
                Json[key] = value;
            }
        }

        nlohmann::ordered_json Json;
    };

    template <>
    inline bool JsonValue::Get(const StringView& name, JsonValue& outValue) const
    {
        if (const auto& it = Json.find(name.data()); it != Json.end())
        {
            outValue.Json = it.value();
            return true;
        }

        return false;
    }

    // for now we only support a raw json format but in the future we want to support an optimized binary format
    class OnyxFile
    {
    public:
        enum class Type : onyxS8
        {
            Invalid,
            Raw,
            Binary,
            Json,
        };

        OnyxFile(const Filepath& filePath);

        ONYX_NO_DISCARD static bool ReadAll(const Filepath& filePath, String& outFileContent);
        ONYX_NO_DISCARD static bool ReadAll(const Filepath& filePath, String& outFileContent, bool shouldSkipBOM);
        ONYX_NO_DISCARD FileStream OpenStream(OpenMode mode) const; // todo make base stream class?

        JsonValue LoadJson() const;
        void WriteJson(const JsonValue& json) const;

    private:
        onyxU64 m_FileId; // file id get created from the path and has to be unique
        Filepath m_FilePath;
    };
}
