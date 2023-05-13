#pragma once
#include "Core/SampleApp.h"
#include "Core/API/VAO.h"
#include "Core/Program/ComputeProgram.h"
#include "Core/Program/GraphicsProgram.h"
#include "Core/Program/ProgramVars.h"
#include "Core/State/ComputeState.h"
#include "Core/State/GraphicsState.h"
#include "Scene/TriangleMesh.h"
#include "Scene/Camera/Camera.h"
#include "Scene/Camera/CameraController.h"

namespace Falcor::Tutorial
{
    class ParametircSurfaceRenderer final : public SampleApp
    {
    public:
        struct Vertex
        {
            float3 position;
            float3 normal;
            float2 texCoord;

            uint32_t modelIndex;
        };

        struct RenderSettings
        {
            bool showFPS = true;
            RasterizerState::FillMode fillMode = RasterizerState::FillMode::Solid;
            RasterizerState::CullMode cullMode = RasterizerState::CullMode::Back;
            float aspectRatio = 1280.f/720.f;
            size_t parametricSurfaceResolution = 10;
        };

        struct DirectionalLightSettings
        {
            float3 ambient = {0.2f, 0.3f, 0.5f};
            float3 diffuse = {0.2f, 0.3f, 0.5f};
            float3 specular = {0.2f, 0.3f, 0.5f};

            float3 lightDir = {0.2f, -0.3f, 0.5f};
        };

        struct ModelSettings
        {
            float3 ambient = {0.2f, 0.3f, 0.5f};
            float3 diffuse = {0.2f, 0.3f, 0.5f};
            float3 specular = {0.2f, 0.3f, 0.5f};

            float4x4 transform;

            float3 position = float3(0, 0, 0);
            float3 scale = float3(1, 1, 1);
            float3 rotation = float3(0, 0, 0);

            Texture::SharedPtr texture = nullptr;
            Texture::SharedPtr perlinNoise = nullptr;

            float noiseIntensity = 2.5f;
        };

        struct Settings
        {
            RenderSettings renderSettings;
            std::vector<ModelSettings> modelSettings;
            DirectionalLightSettings lightSettings;
        };

        enum ObjectType
        {
            Sphere,
            Plane
        };

        explicit ParametircSurfaceRenderer(const SampleAppConfig& config);

        // SampleApp implementation
        void onLoad(RenderContext* pRenderContext) override;
        void onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo) override;
        void onResize(uint32_t width, uint32_t height) override;
        bool onKeyEvent(const KeyboardEvent& keyEvent) override;
        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        void onGuiRender(Gui* pGui) override;

    private:
        // rendering
        Vao::SharedPtr createVao();
        void generatePerlinNoiseBuffer(size_t modelIndex);

        // settings
        void applyRasterStateSettings() const;

        // models
        bool isEveryModelValid();
        Buffer::SharedPtr generateModelVertexBuffers();
        void generateModelIndexBuffer();

        // parametric surfaces
        void createPlane();
        void createSphere();

        Settings mSettings;

        Camera::SharedPtr mpCamera;
        FirstPersonCameraControllerCommon<false>::SharedPtr mpCameraController;
        std::vector<TriangleMesh::SharedPtr> mpModels;
        Buffer::SharedPtr mpIndexBuffer;
        Sampler::SharedPtr mpTextureSampler;
        Sampler::SharedPtr mpNoiseSampler;

        std::shared_ptr<Device> mpDevice;
        GraphicsState::SharedPtr mpGraphicsState;
        GraphicsVars::SharedPtr mpGraphicsVars;
        GraphicsProgram::SharedPtr mpGraphicsProgram;

        ComputeProgram::SharedPtr mpComputeProgram;
        ComputeState::SharedPtr mpComputeState;
        ComputeVars::SharedPtr mpComputeVars;

        bool mReadyToDraw = false;
        bool mShouldGenerateNewNoise = false;
        int mNewNoiseIndex = -1;

        std::unordered_map<ObjectType, uint64_t> objCount;

        FrameRate mFrameRate;
    };
}
