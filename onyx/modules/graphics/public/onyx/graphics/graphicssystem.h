#pragma once
#include <onyx/assets/asset.h>
#include <onyx/engine/enginesystem.h>

#include <onyx/graphics/graphicsapi.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    enum class ApiType : onyxU8;

    struct WindowSettings;
    class GraphicsApi;
    class Window;

    class GraphicsSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Graphics::GraphicsModule";
        StringId32 GetTypeId() const override { return TypeId; }

        GraphicsSystem();
        ~GraphicsSystem() override;

        void Init(Assets::AssetSystem& assetSystem);
        void Shutdown();

        Window& GetWindow() { return *m_Window; }
        const Window& GetWindow() const { return *m_Window; }

        GraphicsApi& GetGraphicsApi() { return *m_GraphicsApi; }
        const GraphicsApi& GetGraphicsApi() const { return *m_GraphicsApi; }

    private:
        UniquePtr<Window> m_Window;
        UniquePtr<GraphicsApi> m_GraphicsApi;
    };
}

namespace Onyx
{
    template <>
    struct Serialization<Graphics::GraphicsSystem>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicsSystem& settings);
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicsSystem& outSettings);
    };

    template <>
    struct Serialization<Graphics::GraphicSettings>
    {
        static bool Serialize(Serializer& serializer, const Graphics::GraphicSettings& settings);
        static bool Deserialize(const Deserializer& deserializer, Graphics::GraphicSettings& outSettings);
    };
}
