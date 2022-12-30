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
        mpMainPass["MandelbrotPSCB"]["iResolution"] = mSettings.resolution;
        mpMainPass["MandelbrotPSCB"]["iIterations"] = mSettings.iterations;
        mpMainPass["MandelbrotPSCB"]["iPoitionOffset"] = mSettings.positionOffset;
        mpMainPass["MandelbrotPSCB"]["iZoom"] = mSettings.zoom;

        // run final pass
        mpMainPass->execute(pRenderContext, pTargetFbo);
    }

    void DirectXShaders::onResizeSwapChain(uint32_t width, uint32_t height)
    {
        mSettings.resolution = { width, height };
    }

    bool DirectXShaders::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        if (keyEvent.type == KeyboardEvent::Type::KeyPressed || keyEvent.type == KeyboardEvent::Type::KeyRepeated)
        {
            const float step = 1 / (mSettings.zoom * 3);

            switch(keyEvent.key)
            {
            case Input::Key::D:
            case Input::Key::Right:
                if (mSettings.positionOffset.x < 3.0f)
                {
                    mSettings.positionOffset.x += step;
                    return true;
                }
                return false;

            case Input::Key::A:
            case Input::Key::Left:
                if (mSettings.positionOffset.x > -3.0f)
                {
                    mSettings.positionOffset.x -= step;
                    return true;
                }
                return false;

            case Input::Key::W:
            case Input::Key::Up:
                if (mSettings.positionOffset.y > -3.0f)
                {
                    mSettings.positionOffset.y -= step;
                    return true;
                }
                return false;

            case Input::Key::S:
            case Input::Key::Down:
                if (mSettings.positionOffset.y < 3.0f)
                {
                    mSettings.positionOffset.y += step;
                    return true;
                }
                return false;

            default:
                return false;
            }
        }

        return false;
    }

    bool DirectXShaders::onMouseEvent(const MouseEvent& mouseEvent)
    {
        if (mouseEvent.type == MouseEvent::Type::Wheel)
        {
            mSettings.zoom += mouseEvent.wheelDelta.y * (mSettings.zoom / 2);

            return true;
        }

        return false;
    }

    void DirectXShaders::onGuiRender(Falcor::Gui* pGui)
    {
        Gui::Window window(pGui, "Mandelbrot set", { 500, 250 }, { 10, 10 });
        gpFramework->renderGlobalUI(pGui);
        window.text("Move around with w, a, s, d or arrow keys, and zoom with scroll wheel");
        window.slider("Zoom level", mSettings.zoom, 0.33f, 70.f);
        window.slider("Iterations", mSettings.iterations, 5, 2048);
        window.slider("Position", mSettings.positionOffset, -3.0, 3.0);
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
