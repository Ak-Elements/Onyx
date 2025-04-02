#pragma once
#include <onyx/engine/enginesystem.h>

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Graphics
{
    struct WindowSettings;
    class GraphicsApi;
    class Window;

    struct GraphicSettings
    {
        ApiType Api;
        Assets::AssetId DefaultRenderGraph;

        bool IsBindless;
        bool IsTimesamplingEnabled;
        bool IsDebugEnabled;

        bool IsDynamicRenderingEnabled;
    };

    class GraphicsSystem : public IEngineSystem
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem() override;

        void Init(const GraphicSettings& graphicSettings, const WindowSettings& windowSettings, Assets::AssetSystem& assetSystem);
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
