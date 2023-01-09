#pragma once
#include "Falcor.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"

namespace Tutorial
{
    struct MandelbrotGUI
    {
        float           zoom = 1;
        int             iterations = 256;
        Falcor::float2  positionOffset{ 0, 0 };
        Falcor::float2  resolution{ 0, 0 };
    };

    class MandelbrotRenderer final : public Falcor::IRenderer
    {
    public:
        // IRenderer implementation
        void onLoad(Falcor::RenderContext* pRenderContext) override;
        void onFrameRender(Falcor::RenderContext* pRenderContext, const Falcor::Fbo::SharedPtr& pTargetFbo) override;
        void onResizeSwapChain(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const Falcor::KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const Falcor::MouseEvent& mouseEvent) override;
        void onGuiRender(Falcor::Gui* pGui) override;


        // own functions
        Falcor::float2 NormalizedScreenPosToMandelbrotPos(const Falcor::float2& pos) const;

    private:
        Falcor::FullScreenPass::SharedPtr mpMainPass;
        MandelbrotGUI mSettings;
        Falcor::float2 mousePos{ 0, 0 };
    };
}
