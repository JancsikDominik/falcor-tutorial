#pragma once

// from falcor
#include "Falcor.h"
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"
#include "Scene/TriangleMesh.h"

namespace Falcor::Tutorial
{
    class ModelLoader final : public SampleApp
    {
    public:
        struct ModelLoaderSettings
        {
            bool showFPS = true;
            bool useCustomLoader = true;
        };

        explicit ModelLoader(const SampleAppConfig& config);

        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo) override;
        void onResize(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        void onGuiRender(Gui* pGui) override;

    private:
        // mesh loading
        void loadModel(const std::filesystem::path& path);
        void loadModelFalcor(const std::filesystem::path& path);
        void loadModelFromObj(const std::filesystem::path& path);

        Camera::SharedPtr mpCamera;
        FirstPersonCameraControllerCommon<false>::SharedPtr mpCameraController;
        TriangleMesh::SharedPtr mpModel;
        ModelLoaderSettings mSettings;

        Buffer::SharedPtr mpVertexBuffer;
        Vao::SharedPtr mpVao;

        GraphicsState::SharedPtr mpGraphicsState;
        GraphicsVars::SharedPtr mpVars;
        std::shared_ptr<Device> mpDevice;
        GraphicsProgram::SharedPtr mpProgram;
    };
}
