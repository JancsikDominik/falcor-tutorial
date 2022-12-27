#include "DirectXShaders.h"

using namespace Falcor;

namespace Tutorial
{
    void DirectXShaders::onLoad(RenderContext* pRenderContext)
    {
        Program::Desc programDesc;
        programDesc.addShaderLibrary("Samples/DirectXShaders/ShaderBasics.3d.slang").vsEntry("main");
        programDesc.addShaderLibrary("Samples/DirectXShaders/ShaderBasics.ps.slang").psEntry("main");

        mpMainPass = FullScreenPass::create(programDesc);
    }

    void DirectXShaders::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        const float width = (float)pTargetFbo->getWidth();
        const float height = (float)pTargetFbo->getHeight();
        mpMainPass["PixelShadeCB"]["iResolution"] = float2(width, height);

        // run final pass
        mpMainPass->execute(pRenderContext, pTargetFbo);
    }

    void DirectXShaders::onShutdown()
    {
    }

    void DirectXShaders::onResizeSwapChain(uint32_t width, uint32_t height)
    {
    }

    bool DirectXShaders::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        // TODO: panning
        return false;
    }

    bool DirectXShaders::onMouseEvent(const MouseEvent& mouseEvent)
    {
        // TODO: zooming
        return false;
    }

    void DirectXShaders::onHotReload(HotReloadFlags reloaded)
    {
    }
}

int main ()
{
    // TODO: menu with settings
    Tutorial::DirectXShaders::UniquePtr pRenderer = std::make_unique<Tutorial::DirectXShaders>();

    SampleConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.deviceDesc.enableVsync = true;
    config.windowDesc.resizableWindow = true;
    config.windowDesc.title = "DirectX shader basics";

    Sample::run(config, pRenderer);
    return 0;
}
