#pragma once

#include <onyx/nodegraph/nodegraphtypemeta.h>

namespace Onyx::NodeGraph
{
    class NodeGraphTypeRegistry
    {
    public:
        template <typename T> requires HasTypeId<T>
        static void RegisterType()
        {
            constexpr StringId32 serializedTypeId = T::TypeId;
            ONYX_ASSERT(s_TypeMeta.contains(serializedTypeId) == false, "Type is already registered in this context");
            s_TypeMeta[serializedTypeId] = MakeUnique<NodeGraphTypeMeta<T>>(serializedTypeId);
            s_RuntimeToStaticTypeId[static_cast<PinTypeId>(TypeHash<T>())] = serializedTypeId;
        }

        template <typename T, CompileTimeString SerializedTypeId>
        static void RegisterType()
        {
            constexpr StringId32 serializedTypeId(SerializedTypeId);
            ONYX_ASSERT(s_TypeMeta.contains(serializedTypeId) == false, "Type is already registered in this context");
            s_TypeMeta[serializedTypeId] = MakeUnique<NodeGraphTypeMeta<T>>(serializedTypeId);
            s_RuntimeToStaticTypeId[static_cast<PinTypeId>(TypeHash<T>())] = serializedTypeId;
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
