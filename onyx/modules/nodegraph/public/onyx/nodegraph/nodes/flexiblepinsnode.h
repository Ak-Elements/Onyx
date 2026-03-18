#pragma once

#include <onyx/nodegraph/pins/pin.h>
#include <onyx/nodegraph/pins/dynamicpin.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx::node_graph
{
    template <typename NodeType>
    class FlexiblePinsNode : public NodeType
    {
    public:
        FlexiblePinsNode() = default;
        FlexiblePinsNode(const FlexiblePinsNode& other) = delete;

        FlexiblePinsNode(FlexiblePinsNode&& other) noexcept
            : NodeType(std::move(other))
              , m_InputPins(std::move(other.m_InputPins))
              , m_OutputPins(std::move(other.m_OutputPins))
        {
        }

        FlexiblePinsNode& operator=(FlexiblePinsNode&& other) noexcept
        {
            if (this == &other)
                return *this;

            using std::swap;
            swap(*this, other);
            return *this;
        }

        const DynamicArray<UniquePtr<PinBase>>& GetInputs() const { return m_InputPins; }
        const DynamicArray<UniquePtr<PinBase>>& GetOutputs() const { return m_OutputPins; }

        onyxU32 GetInputPinCount() const override { return static_cast<onyxU32>(m_InputPins.size()); }
        onyxU32 GetOutputPinCount() const override { return static_cast<onyxU32>(m_OutputPins.size()); }

        PinBase* GetInputPin(onyxU32 index) override { return m_InputPins[index].get(); }
        const PinBase* GetInputPin(onyxU32 index) const override { return m_InputPins[index].get(); }

        PinBase* GetOutputPin(onyxU32 index) override { return m_OutputPins[index].get(); }
        const PinBase* GetOutputPin(onyxU32 index) const override { return m_OutputPins[index].get(); }

#if ONYX_IS_EDITOR

        std::any CreateDefaultForPin(StringId32 pinId) const override
        {
            auto inputIt = std::find_if(m_InputPins.begin(), m_InputPins.end(), [&](const auto& pin)
            {
                return pin->GetLocalId() == pinId;
            });

            if (inputIt != m_InputPins.end())
                return (*inputIt)->CreateDefault();

            auto outputIt = std::find_if(m_OutputPins.begin(), m_OutputPins.end(), [&](const auto& pin)
            {
                return pin->GetLocalId() == pinId;
            });

            if (outputIt != m_OutputPins.end())
                return (*outputIt)->CreateDefault();

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }

        StringView GetPinName(StringId32 pinId) const override
        {
            const onyxU32 inputPinCount = GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                const PinBase* inputPin = GetInputPin(i);
                if (inputPin->GetLocalId() == pinId)
                    return inputPin->GetLocalIdString();
            }

            const onyxU32 outputPinCount = GetOutputPinCount();
            for (onyxU32 i = 0; i < outputPinCount; ++i)
            {
                const PinBase* outputPin = GetOutputPin(i);
                if (outputPin->GetLocalId() == pinId)
                    return outputPin->GetLocalIdString();
            }

            ONYX_ASSERT(false, "Failed to find pin");
            return "";
        }
#endif

    protected:
        template <typename T>
        void AddInputPin()
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_InputPins.emplace_back(new DynamicPin<T>(format::Format("InputPin_{}", m_InputPins.size())));
        }

        template <typename Pin>
        void AddInPin()
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_InputPins.emplace_back(new Pin());
        }

        template <typename T>
        void AddInputPinAt(onyxU32 index)
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_InputPins.emplace(m_InputPins.begin() + index, new DynamicPin<T>(format::Format("InputPin_{}", index)));
        }
        
        void RemoveInputPinAt(onyxU32 index)
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_InputPins.erase(m_InputPins.begin() + index);
        }


        //void AddInputPin(PinBase&& pin)
        //{
        //    //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        //    //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        //    m_InputPins.emplace_back(std::move(pin));
        //}

        template <typename T>
        void AddOutputPin()
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_OutputPins.emplace_back(new DynamicPin<T>(format::Format("OutputPin_{}", m_OutputPins.size())));
        }

        template <typename Pin>
        void AddOutPin()
        {
            //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
            //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
            m_OutputPins.emplace_back(new Pin());
        }

        //void AddOutputPin(PinBase&& pin)
        //{
        //    //ONYX_ASSERT(HasInputPin<Pin>() == false, "Pin already registed with the same id as input.");
        //    //ONYX_ASSERT(HasOutputPin<Pin>() == false, "Pin already registed with the same id as output.");
        //    m_OutputPins.emplace_back(std::move(pin));
        //}

    private:
        bool DeserializePins(const Deserializer& deserializer) override
        {
            return DeserializePins<"inputs">(deserializer, m_InputPins) &&
                DeserializePins<"outputs">(deserializer, m_OutputPins);
        }

        template <CompileTimeString Name>
        bool DeserializePins(const Deserializer& deserializer, DynamicArray<UniquePtr<PinBase>>& outPins)
        {
            Guid64 globalPinId;
            Guid64 linkedPinId;

            bool success = deserializer.ReadForEach<Name>([&](const Deserializer& scopedDeserializer)
            {
                StringId32 localPinId;
                if (scopedDeserializer.Read<"localId">(localPinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing localId in json.");
                    return false;
                }

                if (scopedDeserializer.Read<"id">(globalPinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing global id in json.");
                    return false;
                }

                auto pinIt = std::ranges::find_if(outPins, [&](const UniquePtr<PinBase>& pin)
                    {
                        return pin->GetLocalId() == localPinId;
                    });

                PinBase* pin = pinIt != outPins.end() ? pinIt->get() : nullptr;
                if (pin == nullptr)
                {
                    StringId32 typeId;
                    if (scopedDeserializer.Read<"typeId">(typeId) == false)
                    {
                        ONYX_LOG_ERROR("Pin is missing type id in json.");
                        return false;
                    }

                    pin = outPins.emplace_back(CreatePin(typeId, globalPinId, localPinId)).get();

                }
                else
                {
                    pin->SetGlobalId(globalPinId);
                }

                if (scopedDeserializer.Read<"linkedPin">(linkedPinId))
                {
                    pin->ConnectPin(linkedPinId);
                }
                return true;
            });

            return success;
        }

#if ONYX_ASSERT_ENABLED
        //template <PinType Pin>
        /*bool HasInputPin() const
        {
            auto inputIt = std::find_if(m_InputPins.begin(), m_InputPins.end(), [&](const auto& pin)
                {
                    return pin->GetLocalId() == Pin::LocalId;
                });
 
            return (inputIt != m_InputPins.end());
        }

        template <PinType Pin>
        bool HasOutputPin() const
        {
            auto outputIt = std::find_if(m_OutputPins.begin(), m_OutputPins.end(), [&](const auto& pin)
                {
                    return pin->GetLocalId() == Pin::LocalId;
                });

            return outputIt != m_OutputPins.end();
        }*/
#endif

    private:
        DynamicArray<UniquePtr<PinBase>> m_InputPins;
        DynamicArray<UniquePtr<PinBase>> m_OutputPins;
    };
}
