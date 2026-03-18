#include <onyx/inputactions/inputactionsserializer.h>

#include <onyx/input/inputid.h>
#include <onyx/inputactions/bindings/inputbinding.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>
#include <onyx/inputactions/inputaction.h>
#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/inputactions/bindings/inputbindingsfactory.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace onyx
{
    template <>
    struct Serialization<input_actions::InputAction>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputAction& action)
        {
            return serializer.Write<"id">(action.m_Id) &&
                serializer.Write<"type">(action.m_Type) &&
                serializer.Write<"bindings">(action.m_Bindings);
        }

        static bool Deserialize(const Deserializer& deserializer, input_actions::InputAction& outAction)
        {
            if (deserializer.Read<"id">(outAction.m_Id) == false)
            {
                return false;
            }

            if (deserializer.Read<"type">(outAction.m_Type) == false)
            {
                return false;
            }

            if (outAction.m_Type == input_actions::ActionType::Invalid)
            {
                ONYX_LOG_ERROR("Input action is missing action type.");
                return false;
            }

            return deserializer.Read<"bindings">(outAction.m_Bindings);

        }
    };

    template <>
    struct Serialization<input_actions::InputActionsMap>
    {
        static bool Serialize(Serializer& serializer, const input_actions::InputActionsMap& map)
        {
            serializer.Write(map.m_Actions);
            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, input_actions::InputActionsMap& outMap)
        {
            deserializer.Read(outMap.m_Actions);
            return true;
        }
    };

    template <>
    struct Serialization<UniquePtr<input_actions::InputBinding>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<input_actions::InputBinding>& binding)
        {
            serializer.Write<"typeId">(binding->GetTypeId());

            if (serializer.Write<"triggers">(binding->GetTriggers()) == false)
            {
                return false;
            }

            if (serializer.Write<"modifiers">(binding->GetModifiers()) == false)
            {
                return false;
            }

            const input_actions::InputBindingsFactory::MetaData& metaData = input_actions::InputBindingsFactory::GetMetaData(binding->GetTypeId());
            return metaData.SerializeFunctor(serializer, binding);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<input_actions::InputBinding>& outBinding)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const input_actions::InputBindingsFactory::MetaData& metaData = input_actions::InputBindingsFactory::GetMetaData(typeId);
            outBinding = metaData.CreateFunctor();

            if (deserializer.ReadOptional<"triggers">(outBinding->GetTriggers()) == false)
            {
                return false;
            }

            if (deserializer.ReadOptional<"modifiers">(outBinding->GetModifiers()) == false)
            {
                return false;
            }

            return metaData.DeserializeFunctor(deserializer, outBinding);
        }
    };

    template <>
    struct Serialization<UniquePtr<input_actions::InputTrigger>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<input_actions::InputTrigger>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const input_actions::InputTriggersFactory::MetaData& metaData = input_actions::InputTriggersFactory::GetMetaData(modifier->GetTypeId());
            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<input_actions::InputTrigger>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const input_actions::InputTriggersFactory::MetaData& metaData = input_actions::InputTriggersFactory::GetMetaData(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };

    template <>
    struct Serialization<UniquePtr<input_actions::InputModifier>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<input_actions::InputModifier>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const input_actions::InputModifiersFactory::MetaData& metaData = input_actions::InputModifiersFactory::GetMetaData(modifier->GetTypeId());

            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<input_actions::InputModifier>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const input_actions::InputModifiersFactory::MetaData& metaData = input_actions::InputModifiersFactory::GetMetaData(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };
}

namespace onyx::input_actions
{
    bool InputActionsSerializer::Serialize(const assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& /*meta*/, Serializer& serializer, const IEngine& /*engine*/) const
    {
#if ONYX_IS_EDITOR
        const InputActionsContext& inputActionsAsset = asset.As<InputActionsContext>();
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

    bool InputActionsSerializer::Deserialize(assets::AssetHandle<assets::AssetInterface>& asset, const assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        InputActionsContext& inputAsset = asset.As<InputActionsContext>();

        HashMap<StringId32, InputActionsMap>& contexts = inputAsset.GetMaps();

        contexts.clear();
        inputAsset.SetName(meta.GetName());
        return deserializer.Read(contexts);
    }
}
