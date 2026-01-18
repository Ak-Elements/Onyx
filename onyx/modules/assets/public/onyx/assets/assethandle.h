#pragma once

#include <onyx/assets/assetid.h>

namespace Onyx::Assets
{
    template <typename T>
    class AssetHandle
    {
        template <typename U>
        friend class AssetHandle;

    public:
        AssetHandle() = default;
        AssetHandle(AssetId id, Reference<T> handle) : Id(id), Handle(handle) {}

        AssetHandle(const AssetHandle& other)
            : Id(other.Id)
            , Handle(other.Handle)
        {
        }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        AssetHandle(const AssetHandle<U>& other)
            : Id(other.Id)
            , Handle(other.Handle)
        {
        }


        AssetHandle(AssetHandle&& other) noexcept
            : Id(other.Id)
            , Handle(std::move(other.Handle))
        {
        }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        AssetHandle(AssetHandle<U>&& other) noexcept
            : Id(other.Id)
            , Handle(std::move(other.Handle))
        {
        }

        AssetHandle& operator=(const AssetHandle& other)
        {
            if (this == &other)
                return *this;

            Id = other.Id;
            Handle = other.Handle;
            return *this;
        }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        AssetHandle& operator=(const AssetHandle<U>& other)
        {
            Id = other.Id;
            Handle = other.Handle;
            return *this;
        }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        AssetHandle& operator=(AssetHandle<U>&& other) noexcept
        {
            Id = other.Id;
            Handle = std::move(other.Handle);
            return *this;
        }

        AssetHandle& operator=(AssetHandle&& other) noexcept
        {
            if (this == &other)
                return *this;

            Id = other.Id;
            Handle = std::move(other.Handle);
            return *this;
        }

        bool operator==(const AssetHandle& other) const { return Id == other.Id; }
        bool operator!=(const AssetHandle& other) const { return Id != other.Id; }

        bool HasAssetId() const { return Id != AssetId::Invalid; }
        bool IsValid() const { return Handle.IsValid(); }
        bool IsLoaded() const { return IsValid() && Handle->IsLoaded(); }

        void Reset()
        {
            Id = AssetId::Invalid;
            Handle.Reset();
        }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        U& As() { return Handle.template As<U>(); }

        template <typename U> requires std::is_base_of_v<T, U> || std::is_base_of_v<U, T>
        const U& As() const { return Handle.template As<U>(); }

        T* operator->() { return Handle.Raw(); }
        const T* operator->() const { return Handle.Raw(); }

        T& operator*() { return *Handle; }
        const T& operator*() const { return *Handle; }

        AssetId GetId() const { return Id; }
        void SetId(AssetId id)
        {
            if (id != Id)
            {
                Id = id;
                Handle.Reset();
            }
        }

        Reference<T>& GetHandle() { return Handle; }

    private:
        AssetId Id;
        Reference<T> Handle;
    };
}
