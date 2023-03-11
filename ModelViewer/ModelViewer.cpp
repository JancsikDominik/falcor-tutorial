#include "ModelViewer.h"


namespace Falcor::Tutorial
{

    void ModelViewer::onLoad(RenderContext* pRenderContext)
    {
        Program::Desc programDesc;
        programDesc.addShaderLibrary("Samples/ModelViewer/ModelViewer.vs.slang").vsEntry("main");
        programDesc.addShaderLibrary("Samples/ModelViewer/ModelViewer.ps.slang").psEntry("main");

        mpMainPass = FullScreenPass::create(getDevice(), programDesc);
    }

    void ModelViewer::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        // passing inputs to shaders before rendering
        mSettings.resolution = {static_cast<float>(pTargetFbo->getWidth()), static_cast<float>(pTargetFbo->getHeight())};
        mpMainPass["ModelViewerPSCB"]["iResolution"] = mSettings.resolution;
        mpMainPass["ModelViewerPSCB"]["iPoitionOffset"] = mSettings.positionOffset;
        mpMainPass["ModelViewerPSCB"]["iZoom"] = mSettings.zoom;

        // run final pass
        mpMainPass->execute(pRenderContext, pTargetFbo);
    }

    void ModelViewer::onResize(uint32_t width, uint32_t height)
    {
        mSettings.resolution = { width, height };
    }

    bool ModelViewer::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        return SampleApp::onKeyEvent(keyEvent);
    }

    bool ModelViewer::onMouseEvent(const MouseEvent& mouseEvent)
    {
        return SampleApp::onMouseEvent(mouseEvent);
    }

    void ModelViewer::onGuiRender(Gui* pGui)
    {
        
    }

}

int main()
{
    Falcor::SampleAppConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.windowDesc.title = "Model Viewer";

    Falcor::Tutorial::ModelViewer modelViewer(config);
    return modelViewer.run();
}
