#pragma once

#include <onyx/entity/entityregistry.h>

#include "entitycomponentsystem.h"

namespace Onyx::Entity
{
    class EntityRegistry;
}

namespace Onyx::Graphics
{
    struct FrameContext;
}

namespace Onyx::Entity
{
    template <typename T, typename... Other>
    struct EntityQuery
    {
        EntityQuery(EntityRegistry& registry)
            : m_EntityRegistry(&registry){}

    public:

        template <typename... Excludes>
        ONYX_NO_DISCARD auto GetView(Excludes&&... excludes) const
        {
            return m_EntityRegistry->GetView<T, Other...>(std::forward<Excludes>(excludes)...);
        }

    private:
        EntityRegistry* m_EntityRegistry = nullptr;
    };

    class IDependentFunctionArg
    {
    public:
        IDependentFunctionArg() = default;
        virtual ~IDependentFunctionArg() = default;
    };

    struct ECSExecutionContext
    {
        GameTime DeltaTime;
        EntityRegistry& Registry;
    };

    template <typename T>
    class DependentFunctionArg : public IDependentFunctionArg
    {
    public:
        static T Get(ECSExecutionContext& /*context*/) { return T{}; }
    };

    template <typename T, typename... Others>
    class DependentFunctionArg<EntityQuery<T, Others...>> : public IDependentFunctionArg
    {
    public:
        static EntityQuery<T, Others...> Get(const ECSExecutionContext& context)
        {
            return EntityQuery<T, Others...>(context.Registry);
        }
    };

    template <>
    class DependentFunctionArg<const EntityRegistry&> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static const EntityRegistry& Get(const ECSExecutionContext& context)
        {
            return context.Registry;
        }
    };

    template <>
    class DependentFunctionArg<GameTime> : public IDependentFunctionArg
    {
    public:
        ~DependentFunctionArg() override = default;

        static GameTime Get(const ECSExecutionContext& context)
        {
            return context.DeltaTime;
        }
    };

    class EntityComponentSystemsGraph
    {
    public:
        EntityComponentSystemsGraph() = default;
        ~EntityComponentSystemsGraph() = default;

        void SetRegistry(EntityRegistry* reg)
        {
            m_EntityRegistry = reg;
        }

        template <typename Callable>
        void Register(Callable&& callable)
        {
            m_Systems.emplace_back([&](const ECSExecutionContext& context)
            {
                using FunctionArgs = decltype(GetFunctionArgumentTypes(callable));
                const auto& dependencies = ForEachAndCollect<FunctionArgs>([&]<typename U>() -> U
                {
                    return DependentFunctionArg<U>::Get(context);
                });

                std::apply(callable, dependencies);
            });
        }

        void Update(const ECSExecutionContext& context) const;

    private:

        EntityRegistry* m_EntityRegistry;
        // TODO: DAG?
        DynamicArray<InplaceFunction<void(const ECSExecutionContext&), 64>> m_Systems;
    };
}
