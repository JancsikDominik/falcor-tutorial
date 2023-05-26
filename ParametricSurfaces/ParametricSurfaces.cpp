#include "ParametricSurfaces.h"

#include "Utils/UI/TextRenderer.h"
#include <random>

namespace Falcor::Tutorial
{
    ParametircSurfaceRenderer::ParametircSurfaceRenderer(const SampleAppConfig& config)
        : SampleApp(config)
    {
        mpDevice = getDevice();

        Program::Desc graphicsProgramDesc;
        graphicsProgramDesc.addShaderLibrary("Samples/ParametricSurfaces/ParametricSurfaces.vs.slang").vsEntry("main");
        graphicsProgramDesc.addShaderLibrary("Samples/ParametricSurfaces/ParametricSurfaces.ps.slang").psEntry("main");
        mpGraphicsProgram = GraphicsProgram::create(mpDevice, graphicsProgramDesc);
        mpGraphicsState = GraphicsState::create(mpDevice);
        mpGraphicsState->setProgram(mpGraphicsProgram);
        mpGraphicsVars = GraphicsVars::create(mpDevice, mpGraphicsProgram->getReflector());

        Program::Desc computeProgramDesc;
        computeProgramDesc.addShaderLibrary("Samples/ParametricSurfaces/ParametricSurfaces.cs.slang").csEntry("main");
        mpComputeProgram = ComputeProgram::create(mpDevice, computeProgramDesc);
        mpComputeState = ComputeState::create(mpDevice);
        mpComputeState->setProgram(mpComputeProgram);
        mpComputeVars = ComputeVars::create(mpDevice, mpComputeProgram->getReflector());

        applyRasterStateSettings();

        DepthStencilState::Desc dsDesc;
        dsDesc.setDepthEnabled(true);
        mpGraphicsState->setDepthStencilState(DepthStencilState::create(dsDesc));

        mpTextureSampler = Sampler::create(mpDevice.get(), {});
        mpNoiseSampler = Sampler::create(mpDevice.get(), {});

        mpCamera = Camera::create("main camera");
        mpCamera->setPosition({6, 3, 3});
        mpCamera->setTarget({0, 0, 0});
        mpCamera->setDepthRange(0.1f, 1000.f);

        mpCameraController = FirstPersonCameraController::create(mpCamera);
    }

    void ParametircSurfaceRenderer::onLoad(RenderContext* pRenderContext)
    {
    }

    void ParametircSurfaceRenderer::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        mpCameraController->update();
        pRenderContext->clearFbo(pTargetFbo.get(), {0.1, 0.1, 0.1, 1}, 1.0f, 0, FboAttachmentType::All);

        mpGraphicsVars["VSCBuffer"]["viewProjection"] = mpCamera->getViewProjMatrix();

        // pixel shader cbuffer variables
        mpGraphicsVars["PSCBuffer"]["lightAmbient"] = mSettings.lightSettings.ambient;
        mpGraphicsVars["PSCBuffer"]["lightDiffuse"] = mSettings.lightSettings.diffuse;
        mpGraphicsVars["PSCBuffer"]["lightSpecular"] = mSettings.lightSettings.specular;
        mpGraphicsVars["PSCBuffer"]["lightDir"] = mSettings.lightSettings.lightDir;
        mpGraphicsVars["PSCBuffer"]["cameraPosition"] = mpCamera->getPosition();

        for (int i = 0; i < mSettings.modelSettings.size(); i++)
        {
            mpGraphicsVars["VSCBuffer"]["settings"][i]["transform"] = mSettings.modelSettings[i].transform;
            mpGraphicsVars["VSCBuffer"]["settings"][i]["transformIT"] = inverse(transpose(mSettings.modelSettings[i].transform));
            mpGraphicsVars["VSCBuffer"]["settings"][i]["hasPerlinNoise"] = mSettings.modelSettings[i].perlinNoise != nullptr && mSettings.modelSettings[i].type == Plane;
            mpGraphicsVars["VSCBuffer"]["settings"][i]["texelWidth"] = 1.f / perlinNoiseResolution;

            if (mSettings.modelSettings[i].perlinNoise != nullptr)
            {
                mpGraphicsVars["VSCBuffer"]["settings"][i]["perlinNoise"] = mSettings.modelSettings[i].perlinNoise;
                mpGraphicsVars["VSCBuffer"]["settings"][i]["noiseSampler"] = mpNoiseSampler;
                mpGraphicsVars["VSCBuffer"]["settings"][i]["noiseIntensity"] = mSettings.modelSettings[i].noiseIntensity;
            }

            mpGraphicsVars["PSCBuffer"]["modelSettings"][i]["ambient"] = mSettings.modelSettings[i].ambient;
            mpGraphicsVars["PSCBuffer"]["modelSettings"][i]["diffuse"] = mSettings.modelSettings[i].diffuse;
            mpGraphicsVars["PSCBuffer"]["modelSettings"][i]["specular"] = mSettings.modelSettings[i].specular;
            const bool hasTexture = mSettings.modelSettings[i].texture != nullptr;
            mpGraphicsVars["PSCBuffer"]["modelSettings"][i]["hasTexture"] = hasTexture;

            if (hasTexture)
            {
                mpGraphicsVars["PSCBuffer"]["modelSettings"][i]["tex"] = mSettings.modelSettings[i].texture;
                mpGraphicsVars["PSCBuffer"]["texSampler"] = mpTextureSampler;
            }
        }

        mpGraphicsState->setFbo(pTargetFbo);

        if (mReadyToDraw)
            pRenderContext->drawIndexed(mpGraphicsState.get(), mpGraphicsVars.get(), mpIndexBuffer->getElementCount(), 0, 0);

        if (mShouldGenerateNewNoise)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> seed(0, 10000);

            mpComputeVars["CSCBuffer"]["res"] = static_cast<float>(perlinNoiseResolution);
            mpComputeVars["CSCBuffer"]["seed"] = static_cast<float>(seed(gen));
            mpComputeVars->setTexture("result", mSettings.modelSettings[mNewNoiseIndex].perlinNoise);
            mpComputeProgram->dispatchCompute(
                pRenderContext, mpComputeVars.get(), uint3(perlinNoiseResolution / 16, perlinNoiseResolution / 16, 1)
            );

            mShouldGenerateNewNoise = false;
            mNewNoiseIndex = -1;
        }

        mFrameRate.newFrame();
        if (mSettings.renderSettings.showFPS)
            TextRenderer::render(pRenderContext, mFrameRate.getMsg(), pTargetFbo, {10, 10});
    }

    void ParametircSurfaceRenderer::onResize(uint32_t width, uint32_t height)
    {
        const float h = static_cast<float>(height);
        const float w = static_cast<float>(width);

        if (mpCamera != nullptr)
        {
            mpCamera->setFocalLength(30.f);
            mSettings.renderSettings.aspectRatio = w / h;
            mpCamera->setAspectRatio(mSettings.renderSettings.aspectRatio);
        }
    }

    bool ParametircSurfaceRenderer::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        return mpCameraController->onKeyEvent(keyEvent);
    }

    bool ParametircSurfaceRenderer::onMouseEvent(const MouseEvent& mouseEvent)
    {
        return mpCameraController->onMouseEvent(mouseEvent);
    }

    void ParametircSurfaceRenderer::onGuiRender(Gui* pGui)
    {
        Gui::Window window(pGui, "Settings", {375, 275}, {0, 30});

        static const Gui::DropdownList cullModeList = {
            {static_cast<uint32_t>(RasterizerState::CullMode::Front), "Front"},
            {static_cast<uint32_t>(RasterizerState::CullMode::Back), "Back"},
            {static_cast<uint32_t>(RasterizerState::CullMode::None), "None"}
        };

        if (window.dropdown("Cull mode", cullModeList, reinterpret_cast<uint32_t&>(mSettings.renderSettings.cullMode)))
            applyRasterStateSettings();

        static const Gui::DropdownList fillModeList = {
            {static_cast<uint32_t>(RasterizerState::FillMode::Solid), "Solid"},
            {static_cast<uint32_t>(RasterizerState::FillMode::Wireframe), "Wire-frame"}
        };

        if (window.dropdown("Fill mode", fillModeList, reinterpret_cast<uint32_t&>(mSettings.renderSettings.fillMode)))
            applyRasterStateSettings();

        window.checkbox("Show fps", mSettings.renderSettings.showFPS);

        if (window.button("Generate sphere"))
            createSphere();

        if (window.button("Generate plane"))
            createPlane();

        if (auto lightGroup = window.group("Directional light settings"))
        {
            window.rgbColor("light ambient", mSettings.lightSettings.ambient);
            window.rgbColor("light diffuse", mSettings.lightSettings.diffuse);
            window.rgbColor("light specular", mSettings.lightSettings.specular);
            window.var("light direction", mSettings.lightSettings.lightDir);
        }

        for (size_t i = 0; i < mSettings.modelSettings.size(); i++)
        {
            if (auto modelGroup = window.group(mpModels[i]->getName()))
            {
                window.rgbColor((mpModels[i]->getName() + " ambient").c_str(), mSettings.modelSettings[i].ambient);
                window.rgbColor((mpModels[i]->getName() + " diffuse").c_str(), mSettings.modelSettings[i].diffuse);
                window.rgbColor((mpModels[i]->getName() + " specular").c_str(), mSettings.modelSettings[i].specular);

                window.separator();

                bool transformChanged = false;

                if (window.var((mpModels[i]->getName() + " position").c_str(), mSettings.modelSettings[i].position))
                    transformChanged = true;
                if (window.var((mpModels[i]->getName() + " scale").c_str(), mSettings.modelSettings[i].scale))
                    transformChanged = true;
                if (window.var((mpModels[i]->getName() + " rotation (radian)").c_str(), mSettings.modelSettings[i].rotation))
                    transformChanged = true;

                window.separator();

                window.var(("Noise intensity for " + mpModels[i]->getName()).c_str(), mSettings.modelSettings[i].noiseIntensity);

                if (window.button(("Generate perlin noise for " + mpModels[i]->getName()).c_str()))
                {
                    mShouldGenerateNewNoise = true;
                    mNewNoiseIndex = i;
                    generatePerlinNoiseBuffer(i);
                }

                if (window.button(("Upload texture for " + mpModels[i]->getName()).c_str()))
                {
                    std::filesystem::path path;
                    if (openFileDialog({{"png", ""}, {"jpg", ""}}, path))
                    {
                        mSettings.modelSettings[i].texture = Texture::createFromFile(mpDevice.get(), path, true, false);
                    }
                }

                if (transformChanged)
                {
                    Transform newTransform;

                    newTransform.setScaling(mSettings.modelSettings[i].scale);
                    newTransform.setTranslation(mSettings.modelSettings[i].position);
                    newTransform.setRotationEuler(mSettings.modelSettings[i].rotation);

                    mSettings.modelSettings[i].transform = newTransform.getMatrix();
                }
            }   
        }
    }

    Vao::SharedPtr ParametircSurfaceRenderer::createVao()
    {
        if (mpModels.empty())
            return nullptr;

        if (!isEveryModelValid())
            return nullptr;

        mReadyToDraw = false;

        // batching the models together, so we only need one draw call
        const Buffer::SharedPtr vertexBuffer = generateModelVertexBuffers();
        generateModelIndexBuffer();

        const VertexLayout::SharedPtr pLayout = VertexLayout::create();
        const VertexBufferLayout::SharedPtr pBufLayout = VertexBufferLayout::create();
        pBufLayout->addElement("POSOBJ", offsetof(Vertex, position), ResourceFormat::RGB32Float, 1, 0);
        pBufLayout->addElement("NORMAL", offsetof(Vertex, normal), ResourceFormat::RGB32Float, 1, 1);
        pBufLayout->addElement("TEXCOORD", offsetof(Vertex, texCoord), ResourceFormat::RG32Float, 1, 2);
        pBufLayout->addElement("MODELINDEX", offsetof(Vertex, modelIndex), ResourceFormat::R32Uint, 1, 3);
        pLayout->addBufferLayout(0, pBufLayout);

        Vao::SharedPtr pVao = Vao::create(Vao::Topology::TriangleList, pLayout, {vertexBuffer}, mpIndexBuffer, ResourceFormat::R32Uint);
        mReadyToDraw = true;
        return pVao;
    }

    void ParametircSurfaceRenderer::generatePerlinNoiseBuffer(const size_t modelIndex)
    {
        mSettings.modelSettings[modelIndex].perlinNoise = Texture::create2D(
            mpDevice.get(),
            perlinNoiseResolution,
            perlinNoiseResolution,
            ResourceFormat::RGBA16Float,
            1,
            Resource::kMaxPossible,
            nullptr,
            Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess
        );
    }

    void ParametircSurfaceRenderer::applyRasterStateSettings() const
    {
        if (mpGraphicsState == nullptr)
            return;

        RasterizerState::Desc rsDesc;
        rsDesc.setCullMode(mSettings.renderSettings.cullMode);
        rsDesc.setFillMode(mSettings.renderSettings.fillMode);
        mpGraphicsState->setRasterizerState(RasterizerState::create(rsDesc));
    }

    bool ParametircSurfaceRenderer::isEveryModelValid()
    {
        for (const auto& pModel : mpModels)
        {
            if (pModel == nullptr)
            {
                mReadyToDraw = false;
                return false;
            }

            if (pModel->getVertices().empty() || pModel->getIndices().empty())
            {
                mReadyToDraw = false;
                return false;
            }
        }
        return true;
    }

    Buffer::SharedPtr ParametircSurfaceRenderer::generateModelVertexBuffers()
    {
        const ResourceBindFlags bindFlags = Resource::BindFlags::Vertex | ResourceBindFlags::ShaderResource;
        Buffer::SharedPtr joinedBuffer;

        std::vector<Vertex> vertData;
        for (size_t i = 0; i < mpModels.size(); i++)
        {
            for (const auto& vertexData : mpModels[i]->getVertices())
            {
                Vertex v;
                v.position = vertexData.position;
                v.normal = vertexData.normal;
                v.texCoord = vertexData.texCoord;
                v.modelIndex = i;
                vertData.push_back(v);
            }
        }

        Buffer::SharedPtr pBuffer = Buffer::createStructured(
            mpDevice.get(),
            sizeof(Vertex),
            vertData.size(),
            bindFlags,
            Buffer::CpuAccess::None,
            vertData.data()
        );

        return pBuffer;
    }

    void ParametircSurfaceRenderer::generateModelIndexBuffer()
    {
        const ResourceBindFlags bindFlags = Resource::BindFlags::Index | ResourceBindFlags::ShaderResource;
        TriangleMesh::IndexList joinedIndices;
        uint32_t vertexCount = 0;
        for (const auto& pModel : mpModels)
        {
            auto indices = pModel->getIndices();
            std::for_each(
                indices.begin(),
                indices.end(),
                [&](uint32_t& index) { index += vertexCount; }
            );
            vertexCount += pModel->getVertices().size();
            joinedIndices.insert(joinedIndices.end(), indices.begin(), indices.end());
        }

        mpIndexBuffer = Buffer::createStructured(
            mpDevice.get(),
            sizeof(uint32_t),
            joinedIndices.size(),
            bindFlags,
            Buffer::CpuAccess::None,
            joinedIndices.data()
        );
    }

    void ParametircSurfaceRenderer::createPlane()
    {
        if (mpModels.size() >= 32)
            return;

        const auto& plane = TriangleMesh::create();
        plane->setName("plane" + std::to_string(objCount[Plane]));

        const auto& res = mSettings.renderSettings.parametricSurfaceResolution;
        const float3& normal = { 0, 1, 0 };

        for (size_t i = 0; i < res - 1; i++)
        {
            for (size_t j = 0; j < res - 1; j++)
            {
                const float x = static_cast<float>(j);
                const float z = static_cast<float>(i);

                // first vertex of the quad
                float3 pos1 = {x, 0, z};
                const uint32_t vertex1Index = plane->addVertex(pos1, normal, {x / res, z / res});

                // second vertex of the quad
                float3 pos2 = {x, 0, z + 1};
                const uint32_t vertex2Index = plane->addVertex(pos2, normal, {x / res, (z + 1) / res});

                // third vertex of the quad
                float3 pos3 = {x + 1, 0, z + 1};
                const uint32_t vertex3Index = plane->addVertex(pos3, normal, {(x + 1) / res, (z + 1) / res});

                // fourth vertex of the quad
                float3 pos4 = {x + 1, 0, z};
                const uint32_t vertex4Index = plane->addVertex(pos4, normal, {(x + 1) / res, z / res});

                plane->addTriangle(vertex1Index, vertex2Index, vertex3Index);
                plane->addTriangle(vertex4Index, vertex1Index, vertex3Index);
            }
        }

        mpModels.push_back(plane);
        objCount[Plane]++;

        const Vao::SharedPtr pVao = createVao();
        if (pVao != nullptr)
        {
            ModelSettings settings;
            settings.scale = {0.1, 0.1, 0.1};
            Transform t;
            t.setScaling(settings.scale);
            settings.transform = t.getMatrix();

            mSettings.modelSettings.push_back(settings);
            mReadyToDraw = true;
            mpGraphicsState->setVao(pVao);
            mFrameRate.reset();
        }
        else
        {
            mpModels.pop_back();
            objCount[Plane]--;
        }
    }

    void ParametircSurfaceRenderer::createSphere()
    {
        if (mpModels.size() >= 32)
            return;

        const auto& cube = TriangleMesh::createSphere();
        cube->setName("sphere" + std::to_string(objCount[Sphere]));
        mpModels.push_back(cube);
        objCount[Sphere]++;

        const Vao::SharedPtr pVao = createVao();
        if (pVao != nullptr)
        {
            auto settings = ModelSettings();
            settings.type = Sphere;
            mSettings.modelSettings.push_back(settings);
            mReadyToDraw = true;
            mpGraphicsState->setVao(pVao);
            mFrameRate.reset();
        }
    }

}

int main()
{
    Falcor::SampleAppConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.windowDesc.title = "Parametric surface renderer";

    Falcor::Tutorial::ParametircSurfaceRenderer parametircSurfaces(config);
    return parametircSurfaces.run();
}
