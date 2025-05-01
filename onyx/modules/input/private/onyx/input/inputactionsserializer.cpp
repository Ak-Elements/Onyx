#include <onyx/input/inputactionsserializer.h>

#include <onyx/input/inputaction.h>
#include <onyx/input/inputactionsasset.h>
#include <onyx/input/inputbinding.h>

namespace Onyx::Input
{
    bool InputActionsSerializer::Serialize(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool InputActionsSerializer::SerializeJson(const Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
#if ONYX_IS_EDITOR
        const InputActionsAsset& inputActionsAsset = asset.As<InputActionsAsset>();
        const HashMap<onyxU32, InputActionsMap>& contexts = inputActionsAsset.GetMaps();
        FileSystem::JsonValue jsonRoot;

        for (const InputActionsMap& actionMap : contexts | std::views::values)
        {
            FileSystem::JsonValue actionsJsonArray;

            const DynamicArray<InputAction>& actions = actionMap.GetActions();
            for (const InputAction& action : actions)
            {
                FileSystem::JsonValue actionJson;
                actionJson.Set("id", action.GetId());
                actionJson.Set("name", action.GetName());
                actionJson.Set("type", action.GetType());

                FileSystem::JsonValue bindingsJsonArray;

                const DynamicArray<UniquePtr<InputBinding>>& bindings = action.GetBindings();
                for (const UniquePtr<InputBinding>& binding : bindings)
                {
                    FileSystem::JsonValue bindingJson;
                    bindingJson.Set("typeId", binding->GetTypeId());
#if !ONYX_IS_RETAIL
                    bindingJson.Set("typeIdString", binding->GetTypeId().IdString);
#endif

                    bindingJson.Set("inputType", binding->GetInputType());

                    onyxU32 bindingSlotsCount = binding->GetInputBindingSlotsCount();
                    for (onyxU32 i = 0; i < bindingSlotsCount; ++i)
                    {
                        StringView bindingSlotName = binding->GetInputBindingSlotName(i);
                        bindingJson.Set(bindingSlotName.empty() ? "input" : bindingSlotName, binding->GetBoundInputForSlot(i));
                    }

                    bindingsJsonArray.Add(bindingJson);
                }

                actionJson.Set("bindings", bindingsJsonArray);

                actionsJsonArray.Add(actionJson);
            }

            jsonRoot.Set(actionMap.GetName(), actionsJsonArray);
        }

        const String& jsonString = jsonRoot.Json.dump(4);
        using namespace FileSystem;
        OnyxFile inputConfigFile(filePath);
        FileStream stream = inputConfigFile.OpenStream(OpenMode::Write | OpenMode::Text);
        stream.WriteRaw(jsonString.data(), jsonString.size());
#else
        ONYX_UNUSED(asset);
        ONYX_UNUSED(filePath);
#endif
        return true;
    }

    bool InputActionsSerializer::SerializeYaml(const Reference<Assets::AssetInterface>& asset, FileSystem::FileStream& outStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(outStream);
        return true;
    }

    bool InputActionsSerializer::Deserialize(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }

    bool InputActionsSerializer::DeserializeJson(Reference<Assets::AssetInterface>& asset, const FileSystem::Filepath& filePath) const
    {
        InputActionsAsset& inputAsset = asset.As<InputActionsAsset>();

        HashMap<onyxU32, InputActionsMap>& contexts = inputAsset.GetMaps();

        if (filePath.empty())
            return false;

        contexts.clear();

        inputAsset.SetName(FileSystem::Path::GetFileName(filePath));

        using namespace FileSystem;
        OnyxFile inputConfigFile(filePath);
        const JsonValue& inputConfigData = inputConfigFile.LoadJson();

        // TODO: Fix the wrapping of the nlohmann::ordered_json into JsonValue object
        for (auto&& [key, value] : inputConfigData.Json.items())
        {
            onyxU32 contextId = Hash::FNV1aHash32(key);

            if (contexts.contains(contextId))
            {
                ONYX_LOG_ERROR("Duplicate action context id.");
                continue;
            }

            JsonValue inputActionContextJson{ value };
            contexts.try_emplace(contextId, contextId, key, inputActionContextJson);
        }


        return true;
    }

    bool InputActionsSerializer::DeserializeYaml(Reference<Assets::AssetInterface>& asset, const FileSystem::FileStream& inStream) const
    {
        ONYX_UNUSED(asset);
        ONYX_UNUSED(inStream);
        return true;
    }
}
