#pragma once

#include <onyx/nodegraph/pin.h>
#include <onyx/filesystem/onyxfile.h>

#include <any>

namespace Onyx::NodeGraph
{
    struct PrepareContext;
    struct ExecutionContext;
    class PinBase;

    struct ExecutePolicy
    {
    public:
        void Prepare() { OnPrepare(); }
        void Update(ExecutionContext& context) { OnUpdate(context); }
        void Finish() { OnFinished(); }

    private:
        virtual void OnPrepare() { }
        virtual void OnUpdate(ExecutionContext& /*context*/) { }
        virtual void OnFinished() { }
    };

#if ONYX_IS_EDITOR
    enum class PinVisibility : onyxU8
    {
        None = 0,
        InNode = 1,
        InPropertyGrid = 2,
        Default = InNode | InPropertyGrid
    };

#endif

    class Node
    {
    public:
        virtual ~Node() = default;

        // REMOVE TODO: Node should be data / pins only and the logic should be inferred by CRTP?
    public:
        void Prepare(PrepareContext& context) const { OnPrepare(context); }
        void Update(ExecutionContext& context) const { OnUpdate(context); }
        void Finish() { OnFinished(); }

        virtual bool Serialize(FileSystem::JsonValue& json) const;
        virtual bool Deserialize(const FileSystem::JsonValue& json);

    private:
        virtual void OnPrepare(PrepareContext& /*context*/) const { }
        virtual void OnUpdate(ExecutionContext& /*context*/) const { }
        virtual void OnFinished() { }

    public:
        // REMOVE

        const Guid64& GetId() const { return m_Id; }
        void SetId(Guid64 id) { m_Id = id; }

        onyxU32 GetTypeId() const { return m_TypeId; }
        void SetTypeId(onyxU32 typeId) { m_TypeId = typeId; }

        PinBase* GetPinById(Guid64 globalPinId);
        const PinBase* GetPinById(Guid64 globalPinId) const;
        bool HasPin(Guid64 globalPinId) const;

        //TODO: Improve this interface potentially?
        virtual onyxU32 GetInputPinCount() const { return 0; }
        virtual onyxU32 GetOutputPinCount() const { return 0; }

        virtual PinBase* GetInputPin(onyxU32 /*index*/) { ONYX_ASSERT(false, "Not implemenented."); return nullptr; }
        virtual const PinBase* GetInputPin(onyxU32 /*index*/) const { ONYX_ASSERT(false, "Not implemenented."); return nullptr; }
        virtual PinBase* GetOutputPin(onyxU32 /*index*/) { ONYX_ASSERT(false, "Not implemenented."); return nullptr; }
        virtual const PinBase* GetOutputPin(onyxU32 /*index*/) const { ONYX_ASSERT(false, "Not implemenented."); return nullptr; }

    protected:
        virtual bool OnSerialize(FileSystem::JsonValue& /*json*/) const { return true; }
        virtual bool OnDeserialize(const FileSystem::JsonValue& /*json*/) { return true; }

        template <typename PinT> //requires is_specialization_of_v<Pin, PinT>
        Optional<PinT*> GetInputPinByLocalId()
        {
            PinBase* inputPin = GetInputPinByLocalId(PinT::LocalId);
            if (inputPin != nullptr)
            {
                return static_cast<PinT*>(inputPin);
            }

            return {};
        }

        template <typename PinT> //requires is_specialization_of_v<Pin, PinT>
        Optional<const PinT*> GetInputPinByLocalId() const
        {
            const PinBase* inputPin = GetInputPinByLocalId(PinT::LocalId);
            if (inputPin != nullptr)
            {
                return static_cast<const PinT*>(inputPin);
            }

            return {};
        }

        template <typename PinT> //requires is_specialization_of_v<Pin, PinT>
        Optional<PinT*> GetOutputPinByLocalId()
        {
            PinBase* outputPin = GetOutputPinByLocalId(PinT::LocalId);
            if (outputPin != nullptr)
            {
                return static_cast<PinT&>(*outputPin);
            }

            return {};
        }

        template <typename PinT> //requires is_specialization_of_v<Pin, PinT>
        Optional<const PinT*> GetOutputPinByLocalId() const
        {
            const PinBase* outputPin = GetOutputPinByLocalId(PinT::LocalId);
            if (outputPin != nullptr)
            {
                return static_cast<const PinT*>(outputPin);
            }

            return {};
        }

        PinBase* GetInputPinByLocalId(onyxU32 localPinId);
        const PinBase* GetInputPinByLocalId(onyxU32 localPinId) const;

        PinBase* GetOutputPinByLocalId(onyxU32 localPinId);
        const PinBase* GetOutputPinByLocalId(onyxU32 localPinId) const;

    public:
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        const String& GetName() const { return m_EditorMeta.Name; }
        void SetName(const String& name) { m_EditorMeta.Name = name; }

        void SetTypeName(const StringView& typeName) { m_EditorMeta.TypeName = typeName; }
        const String& GetTypeName() const { return m_EditorMeta.TypeName; }
#endif

#if ONYX_IS_EDITOR
        bool DrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData)
        {
            return OnDrawInPropertyGrid(constantPinData);
        }

        PinVisibility GetPinVisibility(onyxU32 localPinId) const { return DoGetPinVisibility(localPinId); }

        void UIDrawNode() { OnUIDrawNode(); }

        virtual StringView GetPinName(onyxU32 /*localPinId*/) const;
        virtual PinVisibility DoGetPinVisibility(onyxU32 /*localPinId*/) const { return PinVisibility::Default; }

        virtual std::any CreateDefaultForPin(onyxU32 pinId) const = 0;

    protected:
        virtual bool OnDrawInPropertyGrid(HashMap<onyxU64, std::any>& constantPinData)
        {
            bool modified = false;
            const onyxU32 inputPinCount = GetInputPinCount();
            for (onyxU32 i = 0; i < inputPinCount; ++i)
            {
                PinBase* inputPin = GetInputPin(i);
                if (inputPin->GetType() == PinTypeId::Execute)
                    continue;

                if (Enums::HasNoneOf(GetPinVisibility(inputPin->GetLocalId()), PinVisibility::InPropertyGrid))
                    continue;
               
                if (inputPin->IsConnected())
                    continue;

                const Guid64 globalId = inputPin->GetGlobalId();
                if (constantPinData.contains(globalId) == false)
                    constantPinData[globalId] = CreateDefaultForPin(inputPin->GetLocalId());;

                inputPin->DrawPropertyPanel(GetPinName(inputPin->GetLocalId()), constantPinData[globalId]);
            }

            return modified;
        }

        virtual void OnUIDrawNode() {}
#endif

    protected:
        Guid64 m_Id;
        onyxU32 m_TypeId = 0; // should be static

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
        struct EditorMetaInfo
        {
            String Name;
            String TypeName;
            // Used for serialize / deserialize of the node
            // Runtime does not need to store this information though as we do not re-serialize nodes
        };

        EditorMetaInfo m_EditorMeta;
#endif
    };

    template <typename... Policies>
    class NodeWithPolicy : public Node, public Policies... {};

    template <typename NodeType>
    class FlexiblePinsNode : public NodeType
    {
    public:
        FlexiblePinsNode() = default;
        FlexiblePinsNode(const FlexiblePinsNode& other) = delete;

        FlexiblePinsNode(FlexiblePinsNode&& other) noexcept
            : Node(std::move(other))
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

        bool Deserialize(const FileSystem::JsonValue& json) override
        {
            // TODO:
            // We need to verify first that a pin with the localId does not exist before adding a new pin
            // to support 'compile' time added pins in the constructor

            json.Get("id", Node::m_Id);

#if ONYX_IS_EDITOR
            json.Get("type", Node::m_EditorMeta.Name);
#endif

            FileSystem::JsonValue inputPinsJsonArray;
            json.Get("inputs", inputPinsJsonArray);
            if (DeserializePins(inputPinsJsonArray, m_InputPins) == false)
            {
                return false;
            }

            FileSystem::JsonValue outputPinsJsonArray;
            json.Get("outputs", outputPinsJsonArray);
            if (DeserializePins(outputPinsJsonArray, m_OutputPins) == false)
            {
                return false;
            }
        
            this->OnDeserialize(json);

            return true;
        }

        bool DeserializePins(const FileSystem::JsonValue& pinsJsonArray, DynamicArray<UniquePtr<PinBase>>& outPins)
        {
            if (pinsJsonArray.Json.empty())
            {
                return true;
            }

            Guid64 globalPinId;
            Guid64 linkedPinId;

            const onyxU32 pinCount = static_cast<onyxU32>(pinsJsonArray.Json.size());
            for (onyxU32 i = 0; i < pinCount; ++i)
            {
                FileSystem::JsonValue pinJson{ pinsJsonArray.Json[i] };

                onyxU32 localPinId;
                if (pinJson.Get("localId", localPinId) == false)
                {
                    ONYX_LOG_ERROR("Pin is missing localId in json.");
                    return false;
                }

                if (pinJson.Get("id", globalPinId) == false)
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
#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
                    String localPinIdString;
                    if (pinJson.Get("localIdString", localPinIdString) == false)
                    {
                        ONYX_LOG_ERROR("Pin is missing localIdString in json.");
                        return false;
                    }
#endif

                    onyxU32 typeId;
                    if (pinJson.Get("type", typeId) == false)
                    {
                        ONYX_LOG_ERROR("Pin is missing type id in json.");
                        return false;
                    }

                    PinTypeId pinTypeId = static_cast<PinTypeId>(typeId);

#if ONYX_IS_DEBUG || ONYX_IS_EDITOR
                    pin = outPins.emplace_back(CreatePin(pinTypeId, globalPinId, localPinId, localPinIdString)).get();
#else
                    pin = outPins.emplace_back(CreatePin(pinTypeId, globalPinId, localPinId)).get();
#endif
                }
                else
                {
                    pin->SetGlobalId(globalPinId);
                }

                if (pinJson.Get("linkedPin", linkedPinId))
                {
                    pin->ConnectPin(linkedPinId);
                }
            }

            return true;
        }
        
#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(onyxU32 pinId) const override
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

        StringView GetPinName(onyxU32 pinId) const override
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
            m_InputPins.emplace_back(new DynamicPin<T>(Format::Format("InputPin_{}", m_InputPins.size())));
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
            m_InputPins.emplace(m_InputPins.begin() + index, new DynamicPin<T>(Format::Format("InputPin_{}", index)));
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
            m_OutputPins.emplace_back(new DynamicPin<T>(Format::Format("OutputPin_{}", m_OutputPins.size())));
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
#if ONYX_ASSERTS_ENABLED
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

    /*class GraphInputNode : public FlexiblePinsNode
    {
        using InPin = Pin<ExecutePin, "In">;
        using OutExecutePin = Pin<ExecutePin, "Execute">;
    public:
        GraphInputNode()
        {
            AddInputPin<InPin>();
            AddOutputPin<OutExecutePin>();
        }
    };*/

    template <typename NodeType, onyxU8 InPinCount, onyxU8 OutPinCount>
    class FixedPinNode : public NodeType
    {
    public:
        onyxU32 GetInputPinCount() const override { return InPinCount; }
        onyxU32 GetOutputPinCount() const override { return OutPinCount; }

        PinBase* GetInputPin(onyxU32 index) override { return InputPins[static_cast<onyxU8>(index)].get(); }
        const PinBase* GetInputPin(onyxU32 index) const override { return InputPins[static_cast<onyxU8>(index)].get(); }

        PinBase* GetOutputPin(onyxU32 index) override { return OutputPins[static_cast<onyxU8>(index)].get(); }
        const PinBase* GetOutputPin(onyxU32 index) const override { return OutputPins[static_cast<onyxU8>(index)].get(); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            const PinBase* inputPin = this->GetInputPinByLocalId(pinId);
            if (inputPin != nullptr)
            {
                return inputPin->CreateDefault();
            }

            const PinBase* outputPin = this->GetOutputPinByLocalId(pinId);
            if (outputPin != nullptr)
            {
                return outputPin->CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }

        StringView GetPinName(onyxU32 pinId) const override
        {
            const PinBase* inputPin = this->GetInputPinByLocalId(pinId);
            if (inputPin != nullptr)
            {
                return inputPin->GetLocalIdString();
            }

            const PinBase* outputPin = this->GetOutputPinByLocalId(pinId);
            if (outputPin != nullptr)
            {
                return outputPin->GetLocalIdString();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return "";
        }
#endif

    protected:
        InplaceArray<UniquePtr<PinBase>, InPinCount> InputPins;
        InplaceArray<UniquePtr<PinBase>, OutPinCount> OutputPins;
    };

    template <typename NodeType, typename InType>
    class FixedPinNode_1_In : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;

        const InPin& GetInputPin() const { return m_Input; }
        onyxU32 GetInputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            ONYX_ASSERT(m_Input.GetLocalId() == pinId);
            ONYX_UNUSED(pinId);
            return m_Input.CreateDefault();
        }
#endif

    protected:
        InPin m_Input;
    };

    template <typename NodeType, typename OutType>
    class FixedPinNode_1_Out : public NodeType
    {
    public:
        using OutPin = Pin<OutType, "OutPin">;

        const OutPin& GetOutputPin() const { return m_Output; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            ONYX_UNUSED(pinId);
            ONYX_ASSERT(m_Output.GetLocalId() == pinId);
            return m_Output.CreateDefault();
        }
#endif

    protected:
        OutPin m_Output;
    };

    template <typename NodeType, typename InType, typename OutType>
    class FixedPinNode_1_In_1_Out : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;
        using OutPin = Pin<OutType, "OutPin">;

        const InPin& GetInputPin() const { return m_Input; }
        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 1; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }

#if ONYX_IS_EDITOR
        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            if (m_Input.GetLocalId() == pinId)
                return m_Input.CreateDefault();

            if (m_Output.GetLocalId() == pinId)
                return m_Output.CreateDefault();

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin m_Input;
        OutPin m_Output;
    };

    template <typename NodeType, typename InType0, typename InType1, typename OutType>
    class FixedPinNode_2_In_1_Out : public NodeType
    {
    public:
        using InPin0 = Pin<InType0, "InPin0">;
        using InPin1 = Pin<InType1, "InPin1">;
        using OutPin = Pin<OutType, "OutPin">;

        const InPin0& GetInputPin0() const { return m_Input0; }
        const InPin1& GetInputPin1() const { return m_Input1; }
        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 2; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 index) override { return index == 0 ? static_cast<PinBase*>(&m_Input0) : static_cast<PinBase*>(&m_Input1); }
        const PinBase* GetInputPin(onyxU32 index) const override { return index == 0 ? static_cast<const PinBase*>(&m_Input0) : static_cast<const PinBase*>(&m_Input1); }
        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }
#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin0::LocalId: return InPin0::LocalIdString;
                case InPin1::LocalId: return InPin1::LocalIdString;
                case OutPin::LocalId: return OutPin::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin0::LocalId: return m_Input0.CreateDefault();
                case InPin1::LocalId: return m_Input1.CreateDefault();
                case OutPin::LocalId: return m_Output.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin0 m_Input0;
        InPin1 m_Input1;
        OutPin m_Output;
    };

    template <typename NodeType, typename InType0, typename InType1,  typename InType2, typename OutType>
    class FixedPinNode_3_In_1_Out : public NodeType
    {
    public:
        using InPin0 = Pin<InType0, "InPin0">;
        using InPin1 = Pin<InType1, "InPin1">;
        using InPin2 = Pin<InType2, "InPin2">;
        using OutPin = Pin<OutType, "OutPin">;

        const InPin0& GetInputPin0() const { return m_Input0; }
        const InPin1& GetInputPin1() const { return m_Input1; }
        const InPin2& GetInputPin2() const { return m_Input2; }
        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 3; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 index) override
        {
            switch (index)
            {
                case 0:
                    return static_cast<PinBase*>(&m_Input0);
                case 1:
                    return static_cast<PinBase*>(&m_Input1);
                case 2:
                    return static_cast<PinBase*>(&m_Input2);
                default:
                    ONYX_ASSERT(false, "Pin index out of range");
                    return static_cast<PinBase*>(&m_Input0);
            }
        }

        const PinBase* GetInputPin(onyxU32 index) const override
        {
            switch (index)
            {
            case 0:
                return static_cast<const PinBase*>(&m_Input0);
            case 1:
                return static_cast<const PinBase*>(&m_Input1);
            case 2:
                return static_cast<const PinBase*>(&m_Input2);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<const PinBase*>(&m_Input0);
            }
        }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }
#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin0::LocalId: return InPin0::LocalIdString;
                case InPin1::LocalId: return InPin1::LocalIdString;
                case InPin2::LocalId: return InPin2::LocalIdString;
                case OutPin::LocalId: return OutPin::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
                case InPin0::LocalId: return m_Input0.CreateDefault();
                case InPin1::LocalId: return m_Input1.CreateDefault();
                case InPin2::LocalId: return m_Input2.CreateDefault();
                case OutPin::LocalId: return m_Output.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin0 m_Input0;
        InPin1 m_Input1;
        InPin2 m_Input2;
        OutPin m_Output;
    };

    template <typename NodeType, typename InType, typename OutType0, typename OutType1>
    class FixedPinNode_1_In_2_Out : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;
        using OutPin0 = Pin<OutType0, "OutPin0">;
        using OutPin1 = Pin<OutType1, "OutPin1">;

        const InPin& GetInputPin() const { return m_Input; }
        const OutPin0& GetOutputPin0() const { return m_Output0; }
        const OutPin1& GetOutputPin1() const { return m_Output1; }

        onyxU32 GetInputPinCount() const override { return 1; }
        onyxU32 GetOutputPinCount() const override { return 2; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }
        PinBase* GetOutputPin(onyxU32 index) override { return index == 0 ? static_cast<PinBase*>(&m_Output0) : static_cast<PinBase*>(&m_Output1); }
        const PinBase* GetOutputPin(onyxU32 index) const override { return index == 0 ? static_cast<const PinBase*>(&m_Output0) : static_cast<const PinBase*>(&m_Output1); }

#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return InPin::LocalIdString;
            case OutPin0::LocalId: return OutPin0::LocalIdString;
            case OutPin1::LocalId: return OutPin1::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return m_Input.CreateDefault();
            case OutPin0::LocalId: return m_Output0.CreateDefault();
            case OutPin1::LocalId: return m_Output1.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin m_Input;
        OutPin0 m_Output0;
        OutPin1 m_Output1;
    };

    template <typename NodeType, typename InType, typename OutType0, typename OutType1, typename OutType2>
    class FixedPinNode_1_In_3_Out : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;
        using OutPin0 = Pin<OutType0, "OutPin0">;
        using OutPin1 = Pin<OutType1, "OutPin1">;
        using OutPin2 = Pin<OutType2, "OutPin2">;

        const InPin& GetInputPin() const { return m_Input; }
        const OutPin0& GetOutputPin0() const { return m_Output0; }
        const OutPin1& GetOutputPin1() const { return m_Output1; }
        const OutPin2& GetOutputPin2() const { return m_Output2; }

        onyxU32 GetInputPinCount() const override { return 1; }
        onyxU32 GetOutputPinCount() const override { return 3; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }

        PinBase* GetOutputPin(onyxU32 index) override
        {
            switch (index)
            {
                case 0:
                    return static_cast<PinBase*>(&m_Output0);
                case 1:
                    return static_cast<PinBase*>(&m_Output1);
                case 2:
                    return static_cast<PinBase*>(&m_Output2);
                default: 
                    ONYX_ASSERT(false, "Pin index out of range");
                    return static_cast<PinBase*>(&m_Output0);
            }
        }

        const PinBase* GetOutputPin(onyxU32 index) const override
        {
            switch (index)
            {
            case 0:
                return static_cast<const PinBase*>(&m_Output0);
            case 1:
                return static_cast<const PinBase*>(&m_Output1);
            case 2:
                return static_cast<const PinBase*>(&m_Output2);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<const PinBase*>(&m_Output0);
            }
        }

#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return InPin::LocalIdString;
            case OutPin0::LocalId: return OutPin0::LocalIdString;
            case OutPin1::LocalId: return OutPin1::LocalIdString;
            case OutPin2::LocalId: return OutPin2::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return m_Input.CreateDefault();
            case OutPin0::LocalId: return m_Output0.CreateDefault();
            case OutPin1::LocalId: return m_Output1.CreateDefault();
            case OutPin2::LocalId: return m_Output2.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin m_Input;
        OutPin0 m_Output0;
        OutPin1 m_Output1;
        OutPin2 m_Output2;
    };

    template <typename NodeType, typename InType, typename OutType0, typename OutType1, typename OutType2, typename OutType3>
    class FixedPinNode_1_In_4_Out : public NodeType
    {
    public:
        using InPin = Pin<InType, "InPin">;
        using OutPin0 = Pin<OutType0, "OutPin0">;
        using OutPin1 = Pin<OutType1, "OutPin1">;
        using OutPin2 = Pin<OutType2, "OutPin2">;
        using OutPin3 = Pin<OutType3, "OutPin3">;

        const InPin& GetInputPin() const { return m_Input; }
        const OutPin0& GetOutputPin0() const { return m_Output0; }
        const OutPin1& GetOutputPin1() const { return m_Output1; }
        const OutPin2& GetOutputPin2() const { return m_Output2; }
        const OutPin3& GetOutputPin3() const { return m_Output3; }

        onyxU32 GetInputPinCount() const override { return 1; }
        onyxU32 GetOutputPinCount() const override { return 4; }

        PinBase* GetInputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Input); }
        const PinBase* GetInputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Input); }

        PinBase* GetOutputPin(onyxU32 index) override
        {
            switch (index)
            {
            case 0:
                return static_cast<PinBase*>(&m_Output0);
            case 1:
                return static_cast<PinBase*>(&m_Output1);
            case 2:
                return static_cast<PinBase*>(&m_Output2);
            case 3:
                return static_cast<PinBase*>(&m_Output3);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<PinBase*>(&m_Output0);
            }
        }

        const PinBase* GetOutputPin(onyxU32 index) const override
        {
            switch (index)
            {
            case 0:
                return static_cast<const PinBase*>(&m_Output0);
            case 1:
                return static_cast<const PinBase*>(&m_Output1);
            case 2:
                return static_cast<const PinBase*>(&m_Output2);
            case 3:
                return static_cast<const PinBase*>(&m_Output3);
            default:
                ONYX_ASSERT(false, "Pin index out of range");
                return static_cast<const PinBase*>(&m_Output0);
            }
        }

#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return InPin::LocalIdString;
            case OutPin0::LocalId: return OutPin0::LocalIdString;
            case OutPin1::LocalId: return OutPin1::LocalIdString;
            case OutPin2::LocalId: return OutPin2::LocalIdString;
            case OutPin3::LocalId: return OutPin3::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin::LocalId: return m_Input.CreateDefault();
            case OutPin0::LocalId: return m_Output0.CreateDefault();
            case OutPin1::LocalId: return m_Output1.CreateDefault();
            case OutPin2::LocalId: return m_Output2.CreateDefault();
            case OutPin3::LocalId: return m_Output3.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin m_Input;
        OutPin0 m_Output0;
        OutPin1 m_Output1;
        OutPin2 m_Output2;
        OutPin3 m_Output3;
    };

    template <typename NodeType, typename InType0, typename InType1, typename InType2, typename InType3, typename OutType>
    class FixedPinNode_4_In_1_Out : public NodeType
    {
    public:
        using InPin0 = Pin<InType0, "InPin0">;
        using InPin1 = Pin<InType1, "InPin1">;
        using InPin2 = Pin<InType2, "InPin2">;
        using InPin3 = Pin<InType3, "InPin3">;

        using OutPin = Pin<OutType, "OutPin">;

        const InPin0& GetInputPin0() const { return m_Input0; }
        const InPin1& GetInputPin1() const { return m_Input1; }
        const InPin2& GetInputPin2() const { return m_Input2; }
        const InPin3& GetInputPin3() const { return m_Input3; }

        const OutPin& GetOutputPin() const { return m_Output; }

        onyxU32 GetInputPinCount() const override { return 4; }
        onyxU32 GetOutputPinCount() const override { return 1; }

        PinBase* GetInputPin(onyxU32 index) override
        {
            switch (index)
            {
                case 0:
                    return static_cast<PinBase*>(&m_Input0);
                case 1:
                    return static_cast<PinBase*>(&m_Input1);
                case 2:
                    return static_cast<PinBase*>(&m_Input2);
                case 3:
                    return static_cast<PinBase*>(&m_Input3);
                default:
                    ONYX_ASSERT(false, "Pin index out of range");
                    return static_cast<PinBase*>(&m_Input0);
            }
        }

        const PinBase* GetInputPin(onyxU32 index) const override
        {
            switch (index)
            {
                case 0:
                    return static_cast<const PinBase*>(&m_Input0);
                case 1:
                    return static_cast<const PinBase*>(&m_Input1);
                case 2:
                    return static_cast<const PinBase*>(&m_Input2);
                case 3:
                    return static_cast<const PinBase*>(&m_Input3);
                default:
                    ONYX_ASSERT(false, "Pin index out of range");
                    return static_cast<const PinBase*>(&m_Input0);
            }
        }

        PinBase* GetOutputPin(onyxU32 /*index*/) override { return static_cast<PinBase*>(&m_Output); }
        const PinBase* GetOutputPin(onyxU32 /*index*/) const override { return static_cast<const PinBase*>(&m_Output); }

#if ONYX_IS_EDITOR
        StringView GetPinName(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return InPin0::LocalIdString;
            case InPin1::LocalId: return InPin1::LocalIdString;
            case InPin2::LocalId: return InPin2::LocalIdString;
            case InPin3::LocalId: return InPin3::LocalIdString;
            case OutPin::LocalId: return OutPin::LocalIdString;
            }

            ONYX_ASSERT(false, "Invalid pin id");
            return "";
        }

        std::any CreateDefaultForPin(onyxU32 pinId) const override
        {
            switch (pinId)
            {
            case InPin0::LocalId: return m_Input0.CreateDefault();
            case InPin1::LocalId: return m_Input1.CreateDefault();
            case InPin2::LocalId: return m_Input2.CreateDefault();
            case InPin3::LocalId: return m_Input3.CreateDefault();
            case OutPin::LocalId: return m_Output.CreateDefault();
            }

            ONYX_ASSERT(false, "Failed to get pin with local id {}", pinId);
            return nullptr;
        }
#endif

    protected:
        InPin0 m_Input0;
        InPin1 m_Input1;
        InPin2 m_Input2;
        InPin3 m_Input3;
        OutPin m_Output;
    };
}
