#pragma once

#include "Mirror.h"
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"
#include "Scene/TriangleMesh.h"
#include "Scene/Camera/Camera.h"
#include "Scene/Camera/CameraController.h"

namespace Falcor::Tutorial
{
    class MirrorRenderer final : public SampleApp
    {
    public:
        class RenderSettings
        {
        public:
            bool onGuiRender(Gui::Window& window);

            bool showFPS = true;
            RasterizerState::FillMode fillMode = RasterizerState::FillMode::Solid;
            RasterizerState::CullMode cullMode = RasterizerState::CullMode::Back;
        };

        class LightSettings
        {
        public:
            void onGuiRender(Gui::Window& window);

            float3 ambient = {0.2f, 0.3f, 0.5f};
            float3 diffuse = {0.2f, 0.3f, 0.5f};
            float3 specular = {0.2f, 0.3f, 0.5f};
            float3 lightDir = {0.2f, -0.3f, 0.5f};
        };

        struct Settings
        {
            RenderSettings renderSettings;
            LightSettings lightSettings;
        };

        explicit MirrorRenderer(const SampleAppConfig& config)
            : SampleApp(config)
        {
        }

        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onResize(uint32_t width, uint32_t height) override;
        void onFrameRender(RenderContext* pRenderContext, const std::shared_ptr<Fbo>& pTargetFbo) override;
        void onGuiRender(Gui* pGui) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;

    private:
        void createMirror(const float3& pos);
        void applyRasterStateSettings() const;

        // rendering
        Sampler::SharedPtr mpTextureSampler;
        std::shared_ptr<Device> mpDevice;

        Object::List mObjects;

        // mirror
        Texture::SharedPtr mpMirrorTexture;
        TriangleMesh::SharedPtr mpMirror;
        GraphicsState::SharedPtr mpGraphicsState;
        GraphicsVars::SharedPtr mpVars;

        // camera
        Camera::SharedPtr mpMainCamera;
        Camera::SharedPtr mpMirrorCamera;
        FirstPersonCameraController::SharedPtr mpCameraController;

        FrameRate mFrameRate;

        Settings mSettings;
    };
}
