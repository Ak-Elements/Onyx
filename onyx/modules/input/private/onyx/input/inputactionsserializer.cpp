#include <onyx/input/inputactionsserializer.h>

#include <onyx/input/inputaction.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputbinding.h>
#include <onyx/input/inputbindingsregistry.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
    template <>
    struct Serialization<Input::InputActionsMap>
    {
        static bool Serialize(Serializer& serializer, const Input::InputActionsMap& map)
        {
            serializer.Write(map.m_Actions);
            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, Input::InputActionsMap& outMap)
        {
            deserializer.Read(outMap.m_Actions);
            return true;
        }
    };

    template <>
    struct Serialization<Input::InputAction>
    {
        static bool Serialize(Serializer& serializer, const Input::InputAction& action)
        {
            return serializer.Write<"id">(action.m_Id) &&
                serializer.Write<"type">(action.m_Type) &&
                serializer.Write<"bindings">(action.m_Bindings);
        }

        static bool Deserialize(const Deserializer& deserializer, Input::InputAction& outAction)
        {
            if (deserializer.Read<"id">(outAction.m_Id) == false)
            {
                return false;
            }

            if (deserializer.Read<"type">(outAction.m_Type) == false)
            {
                return false;
            }

            if (outAction.m_Type == Input::ActionType::Invalid)
            {
                ONYX_LOG_ERROR("Input action is missing action type.");
                return false;
            }

            return deserializer.Read<"bindings">(outAction.m_Bindings);
        }
    };

    template <>
    struct Serialization<UniquePtr<Input::InputBinding>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<Input::InputBinding>& binding)
        {
            if (serializer.Write<"typeId">(binding->GetTypeId()) == false)
            {
                return false;
            }

            if (serializer.Write<"inputType">(binding->GetInputType()) == false)
            {
                return false;
            }

            onyxU32 bindingSlotsCount = binding->GetInputBindingSlotsCount();
            for (onyxU32 i = 0; i < bindingSlotsCount; ++i)
            {
                StringView bindingSlotName = binding->GetInputBindingSlotName(i);
                if (serializer.Write<onyxU32>(bindingSlotName.empty() ? "input" : bindingSlotName, binding->GetBoundInputForSlot(i)) == false)
                {
                    return false;
                }
            }

            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<Input::InputBinding>& outBinding)
        {
            StringId32 bindingTypeId;
            deserializer.Read<"typeId">(bindingTypeId);

            outBinding = Input::InputBindingsRegistry::CreateBinding(bindingTypeId);

            Input::InputType type;
            deserializer.Read<"inputType">(type);
            outBinding->SetInputType(type);

            onyxU32 boundInput;
            onyxU32 bindingSlotsCount = outBinding->GetInputBindingSlotsCount();
            for (onyxU32 i = 0; i < bindingSlotsCount; ++i)
            {
                StringView bindingSlotName = outBinding->GetInputBindingSlotName(i);
                deserializer.Read<onyxU32>(bindingSlotName.empty() ? "input" : bindingSlotName, boundInput);
                outBinding->SetInputBindingSlot(i, boundInput);
            }

            return true;
        }
    };
}

namespace Onyx::Input
{
    bool InputActionsSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer) const
    {
#if ONYX_IS_EDITOR
        const InputActionsAsset& inputActionsAsset = asset.As<InputActionsAsset>();
        const HashMap<StringId32, InputActionsMap>& contexts = inputActionsAsset.GetMaps();

        bool success = serializer.Write(contexts);
        if (success == false)
        {
            return false;
        }
#else
        ONYX_UNUSED(asset);
        ONYX_UNUSED(serializer);
#endif
        return true;
    }

    bool InputActionsSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer) const
    {
        InputActionsAsset& inputAsset = asset.As<InputActionsAsset>();

        HashMap<StringId32, InputActionsMap>& contexts = inputAsset.GetMaps();

        contexts.clear();
        inputAsset.SetName(meta.GetName());
        return deserializer.Read(contexts);
    }
}
