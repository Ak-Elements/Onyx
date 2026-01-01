#include <onyx/input/inputactionsserializer.h>

#include <onyx/input/bindings/inputbinding.h>
#include <onyx/input/modifiers/inputmodifier.h>

#include <onyx/input/inputaction.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputid.h>
#include <onyx/input/bindings/inputbindingsfactory.h>
#include <onyx/input/modifiers/inputmodifiersfactory.h>
#include <onyx/input/triggers/inputtriggersfactory.h>

#include <onyx/serialize/serializer.h>
#include <onyx/serialize/deserializer.h>

namespace Onyx
{
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
    struct Serialization<UniquePtr<Input::InputBinding>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<Input::InputBinding>& binding)
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

            const Input::InputBindingsFactory::MetaData& metaData = Input::InputBindingsFactory::GetBindingMeta(binding->GetTypeId());
            return metaData.SerializeFunctor(serializer, binding);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<Input::InputBinding>& outBinding)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const Input::InputBindingsFactory::MetaData& metaData = Input::InputBindingsFactory::GetBindingMeta(typeId);
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
    struct Serialization<UniquePtr<Input::InputTrigger>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<Input::InputTrigger>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const Input::InputTriggersFactory::MetaData& metaData = Input::InputTriggersFactory::GetBindingMeta(modifier->GetTypeId());
            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<Input::InputTrigger>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const Input::InputTriggersFactory::MetaData& metaData = Input::InputTriggersFactory::GetBindingMeta(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };

    template <>
    struct Serialization<UniquePtr<Input::InputModifier>>
    {
        static bool Serialize(Serializer& serializer, const UniquePtr<Input::InputModifier>& modifier)
        {
            serializer.Write<"typeId">(modifier->GetTypeId());
            const Input::InputModifiersFactory::MetaData& metaData = Input::InputModifiersFactory::GetBindingMeta(modifier->GetTypeId());

            return metaData.SerializeFunctor(serializer, modifier);
        }

        static bool Deserialize(const Deserializer& deserializer, UniquePtr<Input::InputModifier>& outModifier)
        {
            StringId32 typeId;
            deserializer.Read<"typeId">(typeId);

            const Input::InputModifiersFactory::MetaData& metaData = Input::InputModifiersFactory::GetBindingMeta(typeId);
            outModifier = metaData.CreateFunctor();

            return metaData.DeserializeFunctor(deserializer, outModifier);
        }
    };
}

namespace Onyx::Input
{
    bool InputActionsSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& /*meta*/, Serializer& serializer, IEngine& /*engine*/) const
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

    bool InputActionsSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const Assets::AssetMetaData& meta, const Deserializer& deserializer, IEngine& /*engine*/) const
    {
        InputActionsAsset& inputAsset = asset.As<InputActionsAsset>();

        HashMap<StringId32, InputActionsMap>& contexts = inputAsset.GetMaps();

        contexts.clear();
        inputAsset.SetName(meta.GetName());
        return deserializer.Read(contexts);
    }
}
