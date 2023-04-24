#pragma once

// from falcor
#include "Falcor.h"
#include "Core/SampleApp.h"
#include "Scene/TriangleMesh.h"

namespace Falcor::Tutorial
{
    class ModelLoader final : public SampleApp
    {
    public:
        struct DirectionalLightProperties
        {
            float3 ambient = {0.2f, 0.3f, 0.5f};
            float3 diffuse = {0.2f, 0.3f, 0.5f};
            float3 specular = {0.2f, 0.3f, 0.5f};

            float3 lightDir = {0.2f, -0.3f, 0.5f};
        };

        struct ModelProperties
        {
            float3 ambient = {0.2f, 0.3f, 0.5f};
            float3 diffuse = {0.2f, 0.3f, 0.5f};
            float3 specular = {0.2f, 0.3f, 0.5f};

            Transform transform;

            float3 position = float3(0, 0, 0);
            float3 scale = float3(1, 1, 1);
            float3 rotation = float3(0, 0, 0);
        };

        struct ModelLoaderSettings
        {
            bool showFPS = true;
            bool useCustomLoader = true;
            RasterizerState::FillMode fillMode = RasterizerState::FillMode::Solid;
            RasterizerState::CullMode cullMode = RasterizerState::CullMode::Back;
            DirectionalLightProperties lightSettings;
            ModelProperties modelSettings;
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
        void loadModel();
        void loadTexture();
        void loadModelFalcor(const std::filesystem::path& path);
        void loadModelFromObj(const std::filesystem::path& path);

        // rendering
        Vao::SharedPtr createVao() const;

        // settings
        void applyRasterStateSettings() const;

        Camera::SharedPtr mpCamera;
        FirstPersonCameraControllerCommon<false>::SharedPtr mpCameraController;
        TriangleMesh::SharedPtr mpModel;
        Texture::SharedPtr mpTexture;

        Sampler::SharedPtr mpTextureSampler;
        GraphicsState::SharedPtr mpGraphicsState;
        GraphicsVars::SharedPtr mpVars;
        std::shared_ptr<Device> mpDevice;
        GraphicsProgram::SharedPtr mpProgram;
        bool mReadyToDraw = false;

        FrameRate mFrameRate;
        ModelLoaderSettings mSettings;
    };
}
