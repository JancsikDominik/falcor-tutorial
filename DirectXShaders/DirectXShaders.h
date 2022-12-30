#pragma once
#include "Falcor.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"

namespace Tutorial
{
    struct MandelbrotGUI
    {
        float           zoom = 1;
        int             iterations = 64;
        Falcor::float2  positionOffset{ 0, 0 };
        Falcor::float2  resolution{ 0, 0 };
    };

    class DirectXShaders final : public Falcor::IRenderer
    {
    public:
        void onLoad(Falcor::RenderContext* pRenderContext) override;
        void onFrameRender(Falcor::RenderContext* pRenderContext, const Falcor::Fbo::SharedPtr& pTargetFbo) override;
        void onResizeSwapChain(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const Falcor::KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const Falcor::MouseEvent& mouseEvent) override;
        void onGuiRender(Falcor::Gui* pGui) override;

    private:
        Falcor::FullScreenPass::SharedPtr mpMainPass;
        MandelbrotGUI mSettings;
    };
}
