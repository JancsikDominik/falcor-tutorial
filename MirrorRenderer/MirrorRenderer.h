#pragma once

#include "Mirror.h"
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"
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
            : SampleApp(config), mMirrorObj(std::make_shared<RenderToTextureMirror>(float2(1, 1), getDevice().get(), "main mirror"))
        {
            mObjects.push_back(mMirrorObj);
        }

        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onResize(uint32_t width, uint32_t height) override;
        void onFrameRender(RenderContext* pRenderContext, const std::shared_ptr<Fbo>& pTargetFbo) override;
        void onGuiRender(Gui* pGui) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;

    private:
        void renderObjects(RenderContext* pRenderContext, const std::shared_ptr<Fbo>& pTargetFbo, const Camera& camera) const;
        void applyRasterStateSettings() const;

        // rendering
        Sampler::SharedPtr mpTextureSampler;
        std::shared_ptr<Device> mpDevice;
        GraphicsState::SharedPtr mpGraphicsState;
        GraphicsVars::SharedPtr mpMainVars;
        GraphicsProgram::SharedPtr mpMainProgram;

        // Objects
        Object::List mObjects;
        RenderToTextureMirror::SharedPtr mMirrorObj;

        // camera
        Camera::SharedPtr mpMainCamera;
        FirstPersonCameraController::SharedPtr mpCameraController;

        bool isMainCameraUsed = true;

        FrameRate mFrameRate;

        Settings mSettings;
    };
}
