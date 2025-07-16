#pragma once

#include <onyx/nodegraph/pins/pinbase.h>

namespace Onyx::NodeGraph
{
    struct PrepareContext
    {
    public:
        template <typename T>
        void Set(T&& data)
        {
            constexpr onyxU32 typeId = TypeHash<T>;
            Data[typeId] = std::move(data);
        }

        template <typename T>
        T& Get()
        {
            auto inserted = Data.emplace(TypeHash<T>, T{});
            return std::any_cast<T&>(*inserted.first);
        }

        template <typename T>
        const T& Get() const
        {
            constexpr onyxU32 typeId = TypeHash<T>;
            ONYX_ASSERT(Data.contains(typeId));
            return std::any_cast<const T&>(Data.at(typeId));
        }

    private:
        HashMap<onyxU32, std::any> Data;
    };

    struct ExecutionContext
    {
    public:
        ExecutionContext(PrepareContext& prepareContext)
            : m_PrepareContext(&prepareContext)
        {
        }

        struct PinRuntimeMetaData
        {
            bool IsConnected = false;
        };

        struct NodeContext
        {
            HashMap<StringId32, std::any> PinData;
            HashMap<StringId32, PinRuntimeMetaData> PinMetaData;
        };

        template <PinType Pin>
        ONYX_NO_DISCARD typename Pin::DataType& GetPinData()
        {
            ONYX_ASSERT(m_CurrentNodeContext->PinData.contains(Pin::LocalId));
            return std::any_cast<typename Pin::DataType&>(m_CurrentNodeContext->PinData[Pin::LocalId]);
        }

        template <PinType Pin>
        ONYX_NO_DISCARD const typename Pin::DataType& GetPinData() const
        {
            ONYX_ASSERT(m_CurrentNodeContext->PinData.contains(Pin::LocalId));
            return std::any_cast<const typename Pin::DataType&>(m_CurrentNodeContext->PinData.at(Pin::LocalId));
        }

        template <PinType Pin>
        ONYX_NO_DISCARD
        bool IsPinConnected() const
        {
            return m_CurrentNodeContext->PinMetaData.at(Pin::LocalId).IsConnected;
        }

        const PrepareContext& GetPrepareContext() const
        {
            ONYX_ASSERT(m_PrepareContext != nullptr);
            return *m_PrepareContext;
        }

        // debug functions
        void AddNodeContext(onyxU64 id, NodeContext context)
        {
            m_NodeContexts[id] = context;
        }

        const NodeContext& GetNodeContext(Guid64 nodeId) const
        {
            ONYX_ASSERT(m_NodeContexts.contains(nodeId.Get()));
            return m_NodeContexts.at(nodeId.Get());
        }

        NodeContext& SetCurrentNode(Guid64 nodeId)
        {
            ONYX_ASSERT(m_NodeContexts.contains(nodeId.Get()));
            m_CurrentNodeContext = &(m_NodeContexts.at(nodeId.Get()));
            return *m_CurrentNodeContext;
        }

        template <typename T>
        T& Get()
        {
            auto [it, _]= GraphData.emplace(TypeHash<T>(), T{});
            return std::any_cast<T&>(it->second);
        }

        template <typename T>
        const T& Get() const
        {
            constexpr onyxU32 typeId = TypeHash<T>();
            ONYX_ASSERT(GraphData.contains(typeId));
            return std::any_cast<const T&>(GraphData.at(typeId));
        }

    private:
        HashMap<onyxU64, NodeContext> m_NodeContexts;
        HashMap<onyxU32, std::any> GraphData;

        PrepareContext* m_PrepareContext = nullptr; // non owning
        NodeContext* m_CurrentNodeContext = nullptr;
    };
}
