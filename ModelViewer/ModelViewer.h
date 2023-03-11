#pragma once

#include "Falcor.h"
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"

namespace Falcor::Tutorial
{
    struct ModelViewerSettings
    {
        float   zoom = 1;
        float2  positionOffset{ 0, 0 };
        uint2   resolution{ 0, 0 };
    };

    class ModelViewer final : public SampleApp
    {
    public:
        ModelViewer(const SampleAppConfig& config)
            : SampleApp(config)
        {
        }

        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo) override;
        void onResize(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        void onGuiRender(Gui* pGui) override;

    private:
        FullScreenPass::SharedPtr mpMainPass;
        ModelViewerSettings mSettings;
        Camera::SharedPtr mpCamera;
    };
}
