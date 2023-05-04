#pragma once
#include "Falcor.h"
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"

namespace Falcor::Tutorial
{
    struct MandelbrotGUI
    {
        float   zoom = 1;
        int     iterations = 256;
        float2  positionOffset{ 0, 0 };
        float2  resolution{ 0, 0 };
        bool    isStressTesting = false;
    };

    class MandelbrotRenderer : public SampleApp
    {
    public:
        MandelbrotRenderer(const SampleAppConfig& config) : SampleApp(config)
        {
        }

        void dumpIterationCount() const;
        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo) override;
        void onResize(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        void onGuiRender(Gui* pGui) override;


        // own functions
        float2 NormalizedScreenPosToMandelbrotPos(const float2& pos) const;

    protected:
        FullScreenPass::SharedPtr mpMainPass;
        MandelbrotGUI mSettings;
        bool mIsMouseButtonDown = false;
        float2 mPrevMousePos{ 0, 0 };

        FrameRate mFrameRate;
    };
}
