#pragma once

#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/nodegraph/pins/pinmeta.hpp>

namespace Onyx::NodeGraph
{
    template <typename DataT>
    class DynamicPin : public PinBase
    {
    public:
        using DataType = DataT;

        DynamicPin(StringId32 localId)
            : PinBase(Guid64Generator::GetGuid())
            , LocalId(localId)
        {
        }

        DynamicPin(Guid64 globalPinId, const StringId32 localId)
            : PinBase(globalPinId)
            , LocalId(localId)
        {
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
            // output warning
#endif
        }

        StringId32 LocalId = 0;
        static constexpr PinTypeId DataTypeId = static_cast<PinTypeId>(TypeHash<DataT>());

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
    template <typename DataT>
    void DynamicPin<DataT>::DrawPropertyPanel(StringView name, std::any& anyValue) const
    {
        if constexpr (std::is_same_v<DataT, ExecutePin> == false)
        {
            DataT& value = std::any_cast<DataT&>(anyValue);
            PinMetaObject<DataT>::DrawPinInPropertyGrid(name, value);
        }
    }
#endif


    UniquePtr<PinBase> CreatePin(StringId32 typeId, Guid64 globalId, StringId32 localId);
}
