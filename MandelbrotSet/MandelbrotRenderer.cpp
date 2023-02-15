#include "MandelbrotRenderer.h"

namespace Falcor::Tutorial
{
    void MandelbrotRenderer::onLoad(RenderContext* pRenderContext)
    {
        Program::Desc programDesc;
        programDesc.addShaderLibrary("Samples/MandelbrotSet/Mandelbrot.vs.slang").vsEntry("main");
        programDesc.addShaderLibrary("Samples/MandelbrotSet/Mandelbrot.ps.slang").psEntry("main");

        mpMainPass = FullScreenPass::create(programDesc);
    }

    void MandelbrotRenderer::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        // passing inputs to shaders before rendering
        mSettings.resolution = { static_cast<float>(pTargetFbo->getWidth()), static_cast<float>(pTargetFbo->getHeight()) };
        mpMainPass["MandelbrotPSCB"]["iResolution"] = mSettings.resolution;
        mpMainPass["MandelbrotPSCB"]["iIterations"] = mSettings.iterations;
        mpMainPass["MandelbrotPSCB"]["iPoitionOffset"] = mSettings.positionOffset;
        mpMainPass["MandelbrotPSCB"]["iZoom"] = mSettings.zoom;

        // run final pass
        mpMainPass->execute(pRenderContext, pTargetFbo);
    }

    void MandelbrotRenderer::onResizeSwapChain(uint32_t width, uint32_t height)
    {
        mSettings.resolution = { width, height };
    }

    bool MandelbrotRenderer::onKeyEvent(const KeyboardEvent& keyEvent)
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

    bool MandelbrotRenderer::onMouseEvent(const MouseEvent& mouseEvent)
    {
        // panning with mouse
        if (mouseEvent.type == MouseEvent::Type::ButtonDown || mouseEvent.type == MouseEvent::Type::ButtonUp)
        {
            mIsLeftButtonDown = mouseEvent.type == MouseEvent::Type::ButtonDown;
            return true;
        }

        if (mouseEvent.type == MouseEvent::Type::Move && mIsLeftButtonDown)
        {
            const float2& newMousePos = NormalizedScreenPosToMandelbrotPos(mouseEvent.pos);
            mSettings.positionOffset += mPrevMousePos - newMousePos;

            return true;
        }

        mPrevMousePos = NormalizedScreenPosToMandelbrotPos(mouseEvent.pos);

        // zooming to mouse position
        if (mouseEvent.type == MouseEvent::Type::Wheel)
        {
            const float2& mouseBeforeZoom = NormalizedScreenPosToMandelbrotPos(mouseEvent.pos);
            mSettings.zoom += mouseEvent.wheelDelta.y * (mSettings.zoom / 5);
            const float2& mouseAfterZoom = NormalizedScreenPosToMandelbrotPos(mouseEvent.pos);
            mSettings.positionOffset += mouseBeforeZoom - mouseAfterZoom;

            return true;
        }

        return false;
    }

    void MandelbrotRenderer::onGuiRender(Falcor::Gui* pGui)
    {
        Gui::Window window(pGui, "Settings", { 550, 275 }, { 5, 5 });
        gpFramework->renderGlobalUI(pGui);
        window.text("Move around with w, a, s, d or arrow keys.");
        window.text("Zoom with scroll wheel.");
        window.text("Holding the left button down and moving the mouse will pan the image.");
        window.slider("Zoom level", mSettings.zoom, 0.33f, 70.f);
        window.slider("Iterations", mSettings.iterations, 5, 2048);
        window.slider("Position", mSettings.positionOffset, -3.0, 3.0);

        if (window.button("Reset settings"))
        {
            // keeping resolution, resetting everything else
            float2 res = mSettings.resolution;
            mSettings = MandelbrotGUI();
            mSettings.resolution = res;
        }
    }

    float2 MandelbrotRenderer::NormalizedScreenPosToMandelbrotPos(const float2& pos) const
    {
        return { pos.x * 3.5f * (1.0f / mSettings.zoom) - 2.5f + mSettings.positionOffset.x,
                 pos.y * 2.0f * (1.0f / mSettings.zoom) - 1.0f + mSettings.positionOffset.y };
    }
}

int main()
{
    Falcor::Tutorial::MandelbrotRenderer::UniquePtr pRenderer = std::make_unique<Falcor::Tutorial::MandelbrotRenderer>();

    Falcor::SampleConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.deviceDesc.enableVsync = true;
    config.windowDesc.resizableWindow = false;
    config.windowDesc.title = "Mandelbrot set";

    Falcor::Sample::run(config, pRenderer);
    return 0;
}
