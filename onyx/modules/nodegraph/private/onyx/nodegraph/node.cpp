#include <onyx/nodegraph/node.h>

namespace Onyx::NodeGraph
{
    bool Node::Serialize(FileSystem::JsonValue& json) const
    {
        json.Set("id", m_Id);
        json.Set("typeId", GetTypeId());

#if !ONYX_IS_RETAIL
        json.Set("typeIdString", GetTypeId().IdString);
#endif

#if ONYX_IS_EDITOR
        json.Set("type", GetTypeName());
#endif

        const onyxU32 inputPinCount = GetInputPinCount();
        if (inputPinCount != 0)
        {
            FileSystem::JsonValue nodeInputsJsonArray;
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                FileSystem::JsonValue inputPinObj;
                const PinBase* inputPin = GetInputPin(i);
                inputPinObj.Set("id", inputPin->GetGlobalId());
                inputPinObj.Set("localId", inputPin->GetLocalId());

                if (inputPin->GetType() != PinTypeId::Execute)
                {
                    if (inputPin->IsConnected())
                    {
                        inputPinObj.Set("linkedPin", inputPin->GetLinkedPinGlobalId());
                    }
                }

                nodeInputsJsonArray.Add(inputPinObj);
            }

            json.Set("inputs", nodeInputsJsonArray);
        }

        const onyxU32 outputPinCount = GetOutputPinCount();
        if (outputPinCount != 0)
        {
            FileSystem::JsonValue nodeOutputsJsonArray;
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const PinBase* outputPin = GetOutputPin(i);
                FileSystem::JsonValue outputPinObj;
                outputPinObj.Set("id", outputPin->GetGlobalId());
                outputPinObj.Set("localId", outputPin->GetLocalId());

                if (outputPin->IsConnected())
                    outputPinObj.Set("linkedPin", outputPin->GetLinkedPinGlobalId());

                nodeOutputsJsonArray.Add(outputPinObj);
            }

            json.Set("outputs", nodeOutputsJsonArray);
        }

        OnSerialize(json);

        return true;
    }

    bool Node::Deserialize(const FileSystem::JsonValue& json)
    {
        json.Get("id", m_Id);

#if ONYX_IS_EDITOR
        json.Get("type", m_EditorMeta.Name);
#endif

        Guid64 pinId;
        Guid64 linkedPinId;

        FileSystem::JsonValue inputPinsJsonArray;
        json.Get("inputs", inputPinsJsonArray);

        if (inputPinsJsonArray.Json.empty() == false)
        {
            const onyxU32 inputPinCount = static_cast<onyxU32>(inputPinsJsonArray.Json.size());
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                FileSystem::JsonValue inputPinJson{ inputPinsJsonArray.Json[i] };

                onyxU32 localPinId;
                if (inputPinJson.Get("localId", localPinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing localId in json.");
                    return false;
                }

                PinBase* inputPin = GetInputPinByLocalId(localPinId);
                if (inputPin == nullptr)
                {
                    ONYX_LOG_WARNING("Missing pin with LocalId {:x}", localPinId);
                    continue;
                }

                if (inputPinJson.Get("id", pinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing global id in json.");
                    return false;
                }
                
                inputPin->SetGlobalId(pinId);
                
                if (inputPinJson.Get("linkedPin", linkedPinId))
                {
                    inputPin->ConnectPin(linkedPinId);
                }
            }
        }

        FileSystem::JsonValue outputPinsJsonArray;
        json.Get("outputs", outputPinsJsonArray);

        if (outputPinsJsonArray.Json.empty() == false)
        {
            const onyxU32 outputPinCount = static_cast<onyxU32>(outputPinsJsonArray.Json.size());
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                FileSystem::JsonValue outputPinJson{ outputPinsJsonArray.Json[i] };

                onyxU32 localPinId;
                if (outputPinJson.Get("localId", localPinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing localId in json.");
                    return false;
                }

                PinBase* outputPin = GetOutputPinByLocalId(localPinId);
                if (outputPin == nullptr)
                {
                    ONYX_LOG_WARNING("Missing pin with LocalId {:x}", localPinId);
                    continue;
                }

                if (outputPinJson.Get("id", pinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing global id in json.");
                    return false;
                }

                outputPin->SetGlobalId(pinId);
                if (outputPinJson.Get("linkedPin", linkedPinId))
                {
                    outputPin->ConnectPin(linkedPinId);
                }
            }
        }

        OnDeserialize(json);

        return true;
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

    PinBase* Node::GetInputPinByLocalId(onyxU32 pinId)
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

    const PinBase* Node::GetInputPinByLocalId(onyxU32 pinId) const
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

    PinBase* Node::GetOutputPinByLocalId(onyxU32 pinId)
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

    const PinBase* Node::GetOutputPinByLocalId(onyxU32 pinId) const
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
    StringView Node::GetPinName(onyxU32 localPinId) const
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
