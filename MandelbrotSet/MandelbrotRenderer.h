#pragma once
#include "Falcor.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"

namespace Falcor::Tutorial
{
    struct MandelbrotGUI
    {
        float   zoom = 1;
        int     iterations = 256;
        float2  positionOffset{ 0, 0 };
        float2  resolution{ 0, 0 };
    };

    class MandelbrotRenderer final : public IRenderer
    {
    public:
        // IRenderer implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo) override;
        void onResizeSwapChain(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        void onGuiRender(Gui* pGui) override;


        // own functions
        float2 NormalizedScreenPosToMandelbrotPos(const float2& pos) const;

    private:
        FullScreenPass::SharedPtr mpMainPass;
        MandelbrotGUI mSettings;
        bool mIsMouseButtonDown = false;
        float2 mPrevMousePos{ 0, 0 };
    };
}
