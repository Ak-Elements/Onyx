#pragma once

#include <onyx/nodegraph/pins/dynamicpin.h>
#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx::NodeGraph
{
    struct INodeGraphTypeMeta
    {
        constexpr INodeGraphTypeMeta(StringId32 typeId)
            : TypeId(typeId)
        {
        }

        virtual ~INodeGraphTypeMeta() = default;

        virtual UniquePtr<PinBase> CreatePin(Guid64 globalId, StringId32 localId) const = 0;
        virtual bool Serialize(Serializer&, const std::any& dataAny) const = 0;
        virtual bool Deserialize(const Deserializer&, std::any& outDataAny) const = 0;

        StringId32 TypeId;
    };
   
    template <typename T>
    struct NodeGraphTypeMeta : public INodeGraphTypeMeta
    {
    public:
        NodeGraphTypeMeta(StringId32 typeId)
            : INodeGraphTypeMeta(typeId)
        {
        }

        UniquePtr<PinBase> CreatePin(Guid64 globalId, StringId32 localId) const override
        {
            return MakeUnique<DynamicPin<T>>(globalId, localId);
        }

        bool Serialize(Serializer& serializer, const std::any& dataAny) const override
        {
            if constexpr (std::is_same_v<T, ExecutePin>)
            {
                ONYX_LOG_ERROR("Can not serialize execute pin as it does not hold any data.");
                return false;
            }
            else
            {
                const T& value = std::any_cast<const T&>(dataAny);
                return serializer.Write(value);
            }
            
        }

        bool Deserialize(const Deserializer& deserializer, std::any& outDataAny) const override
        {
            if constexpr (std::is_same_v<T, ExecutePin>)
            {
                ONYX_LOG_ERROR("Can not deserialize execute pin as it does not hold any data.");
                return false;
            }
            else
            {
                T outValue;
                if (deserializer.Read(outValue))
                {
                    outDataAny = outValue;
                    return true;
                }

                return false;
            }
        }
    };
}
