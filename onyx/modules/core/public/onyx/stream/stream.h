#pragma once

namespace Onyx
{
    class Stream;

    template<typename T>
    concept HasSerialize = requires(const T & obj, Stream& outStream)
    {
        { obj.Serialize(outStream) } -> std::same_as<void>;
    };

    template<typename T>
    concept HasDeserialize = requires(T & obj, const Stream& inStream)
    {
        { obj.Deserialize(inStream) } -> std::same_as<void>;
    };

    class Stream
    {
    public:
        virtual ~Stream() = default;

        // interface to implement for streams
        virtual bool IsValid() const = 0;
        virtual bool IsEof() const = 0;

        virtual onyxU64 GetPosition() = 0;
        virtual onyxU64 GetPosition() const = 0;
        virtual void SetPosition(onyxU64 position) = 0;
        virtual onyxU64 GetLength() const = 0;

        operator bool() const { return IsValid(); }
        onyxU64 GetRemainingLength() const
        {
            ONYX_ASSERT(GetLength() >= GetPosition());
            return GetLength() - GetPosition();
        }

        void Reset()
        {
            SetPosition(0);
        }

        template <typename T>
        void Peek(T& out)
        {
            onyxU64 position = GetPosition();
            Read(out);
            SetPosition(position);
        }

        void Skip() { SetPosition(GetPosition() + 1); }
        void Skip(onyxU32 count) { SetPosition(GetPosition() + count); }

        // Read Raw interface
        template <typename T>
        void Read(T& out) const
        {
            if constexpr (HasDeserialize<T>)
                out.Deserialize(*this);
            else
                ReadRaw(out);
        }

        void Read(String& outStr) const;
        void Read(String& outStr, onyxU64 length) const;

        template <typename T>
        void Read(StringId<T>& outId) const
        {
            T id;
            Read(id);

#if ONYX_IS_RETAIL
            outId = { id };
#else
            String idString;
            Read(idString);

            outId = { id, idString };
#endif
        }

        template <typename T>
        void Read(DynamicArray<T>& array, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                array.resize(length);
                DoRead(reinterpret_cast<char*>(array.data()), sizeof(T) * length);
            }
        }

        template <template<typename> typename Container, typename T>
        void Read(Container<T>& array, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                array.resize(length);
                for (onyxU64 i = 0; i < length; ++i)
                {
                    Read(array[i]);
                }
            }
        }

        template <typename T>
        void Read(Set<T>& set, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                for (onyxU64 i = 0; i < length; ++i)
                {
                    T element;
                    Read(element);
                    set.emplace(element);
                }
            }
        }

        template <typename KeyT, typename ValueT>
        void Read(HashMap<KeyT, ValueT>& map, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                KeyT key{};
                for (onyxU64 i = 0; i < length; ++i)
                {
                    Read(key);
                    Read(map[key]);
                }
            }
        }

        void ReadRaw(char* data, onyxU64 size)
        {
            DoRead(data, size);
        }

        // Read Raw (binary data)
        template <typename T>
        void ReadRaw(T& out) const
        {
            DoRead(reinterpret_cast<char*>(&out), sizeof(T));
        }

        template <typename T>
        void ReadRaw(DynamicArray<T>& array, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                array.resize(length);
                for (onyxU64 i = 0; i < length; ++i)
                {
                    ReadRaw(array[i]);
                }
            }
        }

        template <typename T, onyxU8 Size>
        void ReadRaw(InplaceArray<T, Size>& array, onyxU64 length = 0)
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                for (onyxU64 i = 0; i < length; ++i)
                {
                    ReadRaw(array[i]);
                }
            }
        }

        template <typename KeyT, typename ValueT>
        void ReadRaw(const HashMap<KeyT, ValueT>& map, onyxU64 length = 0) const
        {
            if (length == 0)
                Read(length);

            if (length != 0)
            {
                //map.reserve(length);
                KeyT key;
                for (onyxU64 i = 0; i < length; ++i)
                {
                    ReadRaw(key);
                    ReadRaw(map[key]);
                }
            }
        }

        // Write interface
        template <typename T>
        void Write(const T& val)
        {
            if constexpr (HasSerialize<T>)
                val.Serialize(*this);
            else
                WriteRaw(val);
        }

        void Write(const String& val);
        void Write(const StringView& val);

        template <typename T>
        void Write(StringId<T> id)
        {
            Write(id.GetId());
#if !ONYX_IS_RETAIL
            Write(id.GetString());
#endif
        }

        template <template<typename> typename Container, typename T>
        void Write(const Container<T>& array, bool writeSize = true)
        {
            if (writeSize)
                Write(static_cast<onyxU64>(array.size()));

            if (array.empty() == false)
            {
                for (const T& element : array)
                {
                    Write(element);
                }
            }
        }

        template <typename KeyT, typename ValueT>
        void Write(const HashMap<KeyT, ValueT>& map, bool writeSize = true)
        {
            if (writeSize)
                Write(static_cast<onyxU64>(map.size()));

            for (const auto& [key, value] : map)
            {
                Write(key);
                Write(value);
            }
        }

        void WriteRaw(const char* data, onyxU64 size)
        {
            DoWrite(data, size);
        }

        // Write Raw (binary data)
        template <typename T>
        void WriteRaw(const T& val)
        {
            DoWrite(reinterpret_cast<const char*>(&val), sizeof(T));
        }

        template <typename T>
        void WriteRaw(const DynamicArray<T>& array, bool writeSize = true)
        {
            if (writeSize)
                Write(static_cast<onyxU64>(array.size()));

            if (array.empty() == false)
                DoWrite(reinterpret_cast<const char*>(array.data()), sizeof(T) * array.size());
        }

        template <typename T, onyxU8 Size>
        void WriteRaw(const InplaceArray<T, Size>& array, bool writeSize = true)
        {
            if (writeSize)
                Write(static_cast<onyxU64>(array.size()));

            if (array.empty() == false)
                DoWrite(reinterpret_cast<const char*>(array.data()), sizeof(T) * array.size());
        }

        template <typename KeyT, typename ValueT>
        void WriteRaw(const HashMap<KeyT, ValueT>& map, bool writeSize = true)
        {
            if (writeSize)
                Write(static_cast<onyxS64>(map.size()));

            for (const auto& [key, value] : map)
            {
                WriteRaw(key);
                WriteRaw(value);
            }
        }

    private:
        virtual void DoRead(char* destination, onyxU64 size) const = 0;
        virtual void DoWrite(const char* data, onyxU64 size) = 0;
    };
}
