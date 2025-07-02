#include <onyx/nodegraph/nodes/node.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx::NodeGraph
{
    bool Node::Serialize(Serializer& serializer) const
    {
        serializer.Write<"id">(GetId());
        serializer.Write<"typeId">(GetTypeId());

        return SerializePins(serializer) && OnSerialize(serializer);
    }

    bool Node::Deserialize(const Deserializer& deserializer)
    {
        deserializer.Read<"id">(m_Id);
        // TypeId is a compile time / class based thing, therefore no deserialize is needed

        return DeserializePins(deserializer) && OnDeserialize(deserializer);
    }

    bool Node::SerializePins(Serializer& serializer) const
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        serializer.WriteForEach<"inputs">([&](Serializer& scopedSerializer, onyxU32 index)
        {
            const PinBase* inputPin = GetInputPin(index);
            scopedSerializer.Write<"id">(inputPin->GetGlobalId());
            scopedSerializer.Write<"localId">(inputPin->GetLocalId());
            scopedSerializer.Write<"typeId">(NodeGraphTypeRegistry::GetSerializedTypeId(inputPin->GetType()));

            if (inputPin->IsConnected())
            {
                scopedSerializer.Write<"linkedPin">(inputPin->GetLinkedPinGlobalId());
            }
            
            return true;
        }, inputPinCount);

        const onyxU32 outputPinCount = GetOutputPinCount();
        serializer.WriteForEach<"outputs">([&](Serializer& scopedSerializer, onyxU32 index)
        {
            const PinBase* outputPin = GetOutputPin(index);
            scopedSerializer.Write<"id">(outputPin->GetGlobalId());
            scopedSerializer.Write<"localId">(outputPin->GetLocalId());
            scopedSerializer.Write<"typeId">(NodeGraphTypeRegistry::GetSerializedTypeId(outputPin->GetType()));

            if (outputPin->IsConnected())
            {
                scopedSerializer.Write<"linkedPin">(outputPin->GetLinkedPinGlobalId());
            }

            return true;
        }, outputPinCount);
        

        return true;
    }

    bool Node::DeserializePins(const Deserializer& deserializer)
    {
        StringId32 localPinId;
        Guid64 pinId;
        Guid64 linkedPinId;

        bool success = true;
        deserializer.ReadForEach<"inputs">([&](const Deserializer& scopedDeserializer)
        {
            if (scopedDeserializer.Read<"localId">(localPinId) == false)
            {
                ONYX_LOG_ERROR("Pin is missing localId in json.");
                success = false;
                return false;
            }

            PinBase* inputPin = GetInputPinByLocalId(localPinId);
            if (inputPin == nullptr)
            {
                ONYX_LOG_WARNING("Missing pin with LocalId {}", localPinId);
                success = false;
                return false;
            }

            if (scopedDeserializer.Read<"id">(pinId) == false)
            {
                ONYX_LOG_ERROR("Pin is missing global id in json.");
                success = false;
                return false;
            }

            inputPin->SetGlobalId(pinId);

            if (scopedDeserializer.Read<"linkedPin">(linkedPinId))
            {
                inputPin->ConnectPin(linkedPinId);
            }

            return true;
        });

        deserializer.ReadForEach<"outputs">([&](const Deserializer& scopedDeserializer)
        {
            if (scopedDeserializer.Read<"localId">(localPinId) == false)
            {
                ONYX_LOG_ERROR("Pin is missing localId in json.");
                success = false;
                return false;
            }

            PinBase* outputPin = GetOutputPinByLocalId(localPinId);
            if (outputPin == nullptr)
            {
                ONYX_LOG_WARNING("Missing pin with LocalId {}", localPinId);
                success = false;
                return false;
            }

            if (scopedDeserializer.Read<"id">(pinId) == false)
            {
                ONYX_LOG_ERROR("Pin is missing global id in json.");
                success = false;
                return false;
            }

            outputPin->SetGlobalId(pinId);

            if (scopedDeserializer.Read<"linkedPin">(linkedPinId))
            {
                outputPin->ConnectPin(linkedPinId);
            }

            return true;
        });

        return success;
    }

    PinBase* Node::GetPinById(Guid64 globalPinId)
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            PinBase* inputPin = GetInputPin(i);
            if (inputPin->GetGlobalId() == globalPinId)
                return inputPin;
        }

        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            PinBase* outputPin = GetOutputPin(i);
            if (outputPin->GetGlobalId() == globalPinId)
                return outputPin;
        }

        return nullptr;
    }

    const PinBase* Node::GetPinById(Guid64 globalPinId) const
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const PinBase* inputPin = GetInputPin(i);
            if (inputPin->GetGlobalId() == globalPinId)
                return inputPin;
        }

        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const PinBase* outputPin = GetOutputPin(i);
            if (GetOutputPin(i)->GetGlobalId() == globalPinId)
                return outputPin;
        }

        return nullptr;
    }

    bool Node::HasPin(Guid64 globalPinId) const
    {
        onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            if (GetInputPin(i)->GetGlobalId() == globalPinId)
                return true;
        }

        onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            if (GetOutputPin(i)->GetGlobalId() == globalPinId)
                return true;
        }

        return false;
    }

    PinBase* Node::GetInputPinByLocalId(StringId32 pinId)
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            PinBase* inputPin = GetInputPin(i);
            if (inputPin->GetLocalId() == pinId)
                return inputPin;
        }

        return nullptr;
    }

    const PinBase* Node::GetInputPinByLocalId(StringId32 pinId) const
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const PinBase* inputPin = GetInputPin(i);
            if (inputPin->GetLocalId() == pinId)
                return inputPin;
        }

        return nullptr;
    }

    PinBase* Node::GetOutputPinByLocalId(StringId32 pinId)
    {
        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            PinBase* outputPin = GetOutputPin(i);
            if (outputPin->GetLocalId() == pinId)
                return outputPin;
        }

        return nullptr;
    }

    const PinBase* Node::GetOutputPinByLocalId(StringId32 pinId) const
    {
        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const PinBase* outputPin = GetOutputPin(i);
            if (outputPin->GetLocalId() == pinId)
                return outputPin;
        }

        return nullptr;
    }

#if ONYX_IS_EDITOR
    StringView Node::GetPinName(StringId32 localPinId) const
    {
        const onyxU32 inputPinCount = GetInputPinCount();
        for (onyxU32 i = 0; i < inputPinCount; ++i)
        {
            const PinBase* inputPin = GetInputPin(i);
            if (inputPin->GetLocalId() == localPinId)
            {
                return inputPin->GetLocalIdString();
            }
        }

        const onyxU32 outputPinCount = GetOutputPinCount();
        for (onyxU32 i = 0; i < outputPinCount; ++i)
        {
            const PinBase* outputPin = GetOutputPin(i);
            if (outputPin->GetLocalId() == localPinId)
            {
                return outputPin->GetLocalIdString();
            }
        }

        return "";
    }
#endif
}
