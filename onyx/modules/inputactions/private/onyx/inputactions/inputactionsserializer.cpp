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

namespace Onyx
{
    template <>
    struct Serialization<InputActions::InputAction>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputAction& action)
        {
            return serializer.Write<"id">(action.m_Id) &&
                serializer.Write<"type">(action.m_Type) &&
                serializer.Write<"bindings">(action.m_Bindings);
        }

        static bool Deserialize(const Deserializer& deserializer, InputActions::InputAction& outAction)
        {
            if (deserializer.Read<"id">(outAction.m_Id) == false)
            {
                return false;
            }

            if (deserializer.Read<"type">(outAction.m_Type) == false)
            {
                return false;
            }

            if (outAction.m_Type == InputActions::ActionType::Invalid)
            {
                ONYX_LOG_ERROR("Input action is missing action type.");
                return false;
            }

            return deserializer.Read<"bindings">(outAction.m_Bindings);

        }
    };

    template <>
    struct Serialization<InputActions::InputActionsMap>
    {
        static bool Serialize(Serializer& serializer, const InputActions::InputActionsMap& map)
        {
            serializer.Write(map.m_Actions);
            return true;
        }

        static bool Deserialize(const Deserializer& deserializer, InputActions::InputActionsMap& outMap)
        {
            deserializer.Read(outMap.m_Actions);
            return true;
        }
    };

    template <>
    struct Serialization<UniquePtr<InputActions::InputBinding>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<InputActions::InputBinding>& binding)
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

            const InputActions::InputBindingsFactory::MetaData& metaData = InputActions::InputBindingsFactory::GetMetaData(binding->GetTypeId());
            return metaData.SerializeFunctor(serializer, binding);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<InputActions::InputBinding>& outBinding)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const InputActions::InputBindingsFactory::MetaData& metaData = InputActions::InputBindingsFactory::GetMetaData(typeId);
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
    struct Serialization<UniquePtr<InputActions::InputTrigger>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<InputActions::InputTrigger>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const InputActions::InputTriggersFactory::MetaData& metaData = InputActions::InputTriggersFactory::GetMetaData(modifier->GetTypeId());
            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<InputActions::InputTrigger>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const InputActions::InputTriggersFactory::MetaData& metaData = InputActions::InputTriggersFactory::GetMetaData(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };

    template <>
    struct Serialization<UniquePtr<InputActions::InputModifier>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<InputActions::InputModifier>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const InputActions::InputModifiersFactory::MetaData& metaData = InputActions::InputModifiersFactory::GetMetaData(modifier->GetTypeId());

            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<InputActions::InputModifier>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const InputActions::InputModifiersFactory::MetaData& metaData = InputActions::InputModifiersFactory::GetMetaData(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };
}

namespace Onyx::InputActions
{
    bool InputActionsSerializer::Serialize(const Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer, const IEngine& /*engine*/) const
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

    bool InputActionsSerializer::Deserialize(Assets::AssetHandle<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        InputActionsContext& inputAsset = asset.As<InputActionsContext>();

        HashMap<StringId32, InputActionsMap>& contexts = inputAsset.GetMaps();

        contexts.clear();
        inputAsset.SetName(meta.GetName());
        return deserializer.Read(contexts);
    }
}
