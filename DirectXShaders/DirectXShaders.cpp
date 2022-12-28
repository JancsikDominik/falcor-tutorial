#include "DirectXShaders.h"

using namespace Falcor;

namespace Tutorial
{
    void DirectXShaders::onLoad(RenderContext* pRenderContext)
    {
        Program::Desc programDesc;
        programDesc.addShaderLibrary("Samples/DirectXShaders/Mandelbrot.vs.slang").vsEntry("main");
        programDesc.addShaderLibrary("Samples/DirectXShaders/Mandelbrot.ps.slang").psEntry("main");

        mpMainPass = FullScreenPass::create(programDesc);
    }

    void DirectXShaders::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        const float width = (float)pTargetFbo->getWidth();
        const float height = (float)pTargetFbo->getHeight();
        mpMainPass["MandelbrotPSCB"]["iResolution"] = float2(width, height);
        mpMainPass["MandelbrotPSCB"]["iIterations"] = mGUISettings.iterations;
        mpMainPass["MandelbrotPSCB"]["iPoitionOffset"] = mGUISettings.positionOffset;
        mpMainPass["MandelbrotPSCB"]["iZoom"] = mGUISettings.zoom;

        // run final pass
        mpMainPass->execute(pRenderContext, pTargetFbo);
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

    void DirectXShaders::onGuiRender(Falcor::Gui* pGui)
    {
        Gui::Window window(pGui, "Mandelbrot set", { 500, 250 }, { 10, 10 });
        gpFramework->renderGlobalUI(pGui);
        window.text("Move around with w, a, s, d or arrow keys, and zoom with scroll wheel");
        window.slider("Zoom level", mGUISettings.zoom, 0.001f, 70.f);
        window.slider("Iterations", mGUISettings.iterations, 1, 2048);
        window.slider("Position", mGUISettings.positionOffset, -3.0, 3.0);
    }
}

int main ()
{
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
