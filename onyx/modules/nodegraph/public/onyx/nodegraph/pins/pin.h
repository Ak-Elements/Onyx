#pragma once

#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/pins/pinmeta.h>

namespace Onyx::NodeGraph
{
    template <typename DataT, CompileTimeString PinId>
    class Pin : public PinBase
    {
    public:
        using DataType = DataT;
        static constexpr StringId32 LocalId { PinId };
        static constexpr PinTypeId DataTypeId = static_cast<PinTypeId>(TypeHash<DataT>());

        Pin()
            : PinBase(Guid64Generator::GetGuid())
        {
        }

        Pin(Guid64 globalPinId)
            : PinBase(globalPinId)
        {
        }

        std::any CreateDefault() const override { return DataT(); }

#if ONYX_IS_EDITOR
        void DrawPropertyPanel(StringView name, std::any& anyValue) const override;
        constexpr onyxU32 GetTypeColor() const override { return PinMetaObject<DataT>::GetPinTypeColor(); }
#endif

        bool Serialize(FileSystem::JsonValue& json, const std::any& anyValue) const override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                const DataT& value = std::any_cast<const DataT&>(anyValue);
                return PinMetaObject<DataT>::Serialize(json, value);
            }
            else
            {
                return true;
            }
        }

        bool Deserialize(const FileSystem::JsonValue& json, std::any& anyValue) override
        {
            if constexpr (std::is_same_v<DataT, ExecutePin> == false)
            {
                DataT data;
                if (PinMetaObject<DataT>::Deserialize(json, data))
                {
                    anyValue = data;
                    return true;
                }

                return false;
            }
            else
            {
                return true;
            }
        }

        StringId32 GetLocalId() const override { return LocalId; }
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        StringView GetLocalIdString() const override { return LocalId.GetString(); }
#endif

        PinTypeId GetType() override { return DataTypeId; }
        PinTypeId GetType() const override { return DataTypeId; }
    };

#if ONYX_IS_EDITOR
    template <typename DataT, CompileTimeString PinId>
    void Pin<DataT, PinId>::DrawPropertyPanel(StringView name, std::any& anyValue) const
    {
        if constexpr(std::is_same_v<DataT, ExecutePin> == false)
        {
            DataT& value = std::any_cast<DataT&>(anyValue);
            PinMetaObject<DataT>::DrawPinInPropertyGrid(name, value);
        }
    }
#endif
}
