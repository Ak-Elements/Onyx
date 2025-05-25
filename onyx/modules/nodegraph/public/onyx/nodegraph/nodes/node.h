#pragma once

#include <onyx/nodegraph/pins/pinbase.h>
#include <onyx/filesystem/onyxfile.h>

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

        void SerializePin(const PinBase& pin, FileSystem::JsonValue& outPinsJsonArray) const; 

    public:
        // REMOVE
        const Guid64& GetId() const { return m_Id; }
        void SetId(Guid64 id) { m_Id = id; }

        virtual StringId32 GetTypeId() const = 0;

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

        PinBase* GetInputPinByLocalId(StringId32 localPinId);
        const PinBase* GetInputPinByLocalId(StringId32 localPinId) const;

        PinBase* GetOutputPinByLocalId(StringId32 localPinId);
        const PinBase* GetOutputPinByLocalId(StringId32 localPinId) const;

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

        PinVisibility GetPinVisibility(StringId32 localPinId) const { return DoGetPinVisibility(localPinId); }

        void UIDrawNode() { OnUIDrawNode(); }

        virtual StringView GetPinName(StringId32 /*localPinId*/) const;
        virtual PinVisibility DoGetPinVisibility(StringId32 /*localPinId*/) const { return PinVisibility::Default; }

        virtual std::any CreateDefaultForPin(StringId32 pinId) const = 0;

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
                    constantPinData[globalId] = CreateDefaultForPin(inputPin->GetLocalId());

                inputPin->DrawPropertyPanel(GetPinName(inputPin->GetLocalId()), constantPinData[globalId]);
            }

            return modified;
        }

        virtual void OnUIDrawNode() {}
#endif

    protected:
        Guid64 m_Id;

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
}
