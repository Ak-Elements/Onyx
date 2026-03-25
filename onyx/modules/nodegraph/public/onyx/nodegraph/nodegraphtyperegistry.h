#pragma once

#include <onyx/nodegraph/nodegraphtypemeta.h>

namespace onyx::node_graph
{
    class NodeGraphTypeRegistry
    {
    public:
        template <typename T> requires HasTypeId<T>
        static void Register()
        {
            constexpr StringId32 serializedTypeId = T::TypeId;
            constexpr PinTypeId pinTypeId = static_cast<PinTypeId>(TypeHash<T>());

            bool hasRegisteredType = s_TypeMeta.contains(serializedTypeId);
#if ONYX_ASSERT_ENABLED
            auto it = s_RuntimeToStaticTypeId.find(pinTypeId);
            
            bool hasFoundTypeId = it != s_RuntimeToStaticTypeId.end();
            bool isMatchingTypeId = (hasFoundTypeId == false) || (it->second == serializedTypeId);
            ONYX_ASSERT((hasRegisteredType == hasFoundTypeId), "Type({}) was not registered with the same PinTypeId({})", serializedTypeId, enums::ToIntegral(pinTypeId));
            ONYX_ASSERT((hasRegisteredType == hasFoundTypeId) && isMatchingTypeId, "Type({}) was registered but a similar type overlaps the PinTypeId({}, typeId:{})", serializedTypeId, enums::ToIntegral(pinTypeId), it->second);
#endif

            if (hasRegisteredType)
            {
                return;
            }

            s_TypeMeta[serializedTypeId] = makeUnique<NodeGraphTypeMeta<T>>(serializedTypeId);
            s_RuntimeToStaticTypeId[pinTypeId] = serializedTypeId;
        }

        template <typename T, CompileTimeString SerializedTypeId>
        static void Register()
        {
            constexpr StringId32 serializedTypeId = SerializedTypeId;
            constexpr PinTypeId pinTypeId = static_cast<PinTypeId>(TypeHash<T>());

            bool hasRegisteredType = s_TypeMeta.contains(serializedTypeId);
#if ONYX_ASSERT_ENABLED
            auto it = s_RuntimeToStaticTypeId.find(pinTypeId);

            bool hasFoundTypeId = it != s_RuntimeToStaticTypeId.end();
            bool isMatchingTypeId = (hasFoundTypeId == false) || (it->second == serializedTypeId);
            ONYX_ASSERT((hasRegisteredType == hasFoundTypeId), "Type({}) was not registered with the same PinTypeId({})", serializedTypeId, enums::ToIntegral(pinTypeId));
            ONYX_ASSERT((hasRegisteredType == hasFoundTypeId) && isMatchingTypeId, "Type({}) was registered but a similar type overlaps the PinTypeId({}, typeId:{})", serializedTypeId, enums::ToIntegral(pinTypeId), it->second);
#endif

            if (hasRegisteredType)
            {
                return;
            }

            s_TypeMeta[serializedTypeId] = makeUnique<NodeGraphTypeMeta<T>>(serializedTypeId);
            s_RuntimeToStaticTypeId[pinTypeId] = serializedTypeId;
        }

        static StringId32 GetSerializedTypeId(PinTypeId pinTypeId)
        {
            return s_RuntimeToStaticTypeId.at(pinTypeId);
        }

        static const INodeGraphTypeMeta& GetTypeMeta(StringId32 typeId)
        {
            UniquePtr<INodeGraphTypeMeta>& typeMeta = s_TypeMeta.at(typeId);
            return *typeMeta;
        }
        
        static const INodeGraphTypeMeta& GetTypeMeta(PinTypeId pinTypeId)
        {
            StringId32 typeId = s_RuntimeToStaticTypeId.at(pinTypeId);
            UniquePtr<INodeGraphTypeMeta>& typeMeta = s_TypeMeta.at(typeId);
            return *typeMeta;
        }

    private:
        static HashMap<StringId32, UniquePtr<INodeGraphTypeMeta>> s_TypeMeta;
        static HashMap<PinTypeId, StringId32> s_RuntimeToStaticTypeId;
    };
}
