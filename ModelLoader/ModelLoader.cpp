#include "ModelLoader.h"

#include "Utils/UI/TextRenderer.h"
#include "MeshLoader.h"


namespace Falcor::Tutorial
{

    ModelLoader::ModelLoader(const SampleAppConfig& config)
        : SampleApp(config)
    {
        mpDevice = getDevice();

        Program::Desc programDesc;
        programDesc.addShaderLibrary("Samples/ModelLoader/ModelLoader.vs.slang").vsEntry("main");
        programDesc.addShaderLibrary("Samples/ModelLoader/ModelLoader.ps.slang").psEntry("main");

        mpProgram = GraphicsProgram::create(mpDevice, programDesc);
        mpGraphicsState = GraphicsState::create(mpDevice);
        mpGraphicsState->setProgram(mpProgram);
        mpVars = GraphicsVars::create(mpDevice, mpProgram->getReflector());

        applyRasterStateSettings();

        DepthStencilState::Desc dsDesc;
        dsDesc.setDepthEnabled(true);
        mpGraphicsState->setDepthStencilState(DepthStencilState::create(dsDesc));

        mpCamera = Camera::create("main camera");
        mpCamera->setPosition({6, 3, 3});
        mpCamera->setTarget({0, 0, 0});
        mpCamera->setDepthRange(0.1f, 1000.f);

        mpCameraController = FirstPersonCameraController::create(mpCamera);

        mpTextureSampler = Sampler::create(mpDevice.get(), {});
    }

    void ModelLoader::onLoad(RenderContext* pRenderContext)
    {
        mpTexture = Texture::createFromFile(mpDevice.get(), "C:/Users/Jancsik/Documents/texture.jpg", true, false);
    }

    void ModelLoader::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        mpCameraController->update();
        pRenderContext->clearFbo(pTargetFbo.get(), {0, 0.25, 0, 1}, 1.0f, 0, FboAttachmentType::All);

        // vertex shader cbuffer variables
        mpVars["VSCBuffer"]["model"] = float4x4(1); // identity matrix
        mpVars["VSCBuffer"]["viewProjection"] = mpCamera->getViewProjMatrix();


        // pixel shader cbuffer variables
        mpVars["PSCBuffer"]["lightAmbient"] = mSettings.lightSettings.ambient;
        mpVars["PSCBuffer"]["lightDiffuse"] = mSettings.lightSettings.diffuse;
        mpVars["PSCBuffer"]["lightSpecular"] = mSettings.lightSettings.specular;
        mpVars["PSCBuffer"]["lightDir"] = mSettings.lightSettings.lightDir;
        mpVars["PSCBuffer"]["materialAmbient"] = mSettings.materialSettings.ambient;
        mpVars["PSCBuffer"]["materialDiffuse"] = mSettings.materialSettings.diffuse;
        mpVars["PSCBuffer"]["materialSpecular"] = mSettings.materialSettings.specular;
        mpVars["PSCBuffer"]["cameraPosition"] = mpCamera->getPosition();
        mpVars["PSCBuffer"]["objTexture"] = mpTexture;
        mpVars["PSCBuffer"]["texSampler"] = mpTextureSampler;

        mpGraphicsState->setFbo(pTargetFbo);

        if (mReadyToDraw)
            pRenderContext->drawIndexed(mpGraphicsState.get(), mpVars.get(), mpModel->getIndices().size(), 0, 0);

        mFrameRate.newFrame();
        if (mSettings.showFPS)
            TextRenderer::render(pRenderContext, mFrameRate.getMsg(), pTargetFbo, {10, 10});
    }

    void ModelLoader::onResize(uint32_t width, uint32_t height)
    {
        const float h = static_cast<float>(height);
        const float w = static_cast<float>(width);

        if (mpCamera != nullptr)
        {
            mpCamera->setFocalLength(30.f);
            const float aspectRatio = (w / h);
            mpCamera->setAspectRatio(aspectRatio);
        }
    }

    bool ModelLoader::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        return mpCameraController->onKeyEvent(keyEvent);
    }

    bool ModelLoader::onMouseEvent(const MouseEvent& mouseEvent)
    {
        return mpCameraController->onMouseEvent(mouseEvent);
    }

    void ModelLoader::onGuiRender(Gui* pGui)
    {
        Gui::Window window(pGui, "Settings", {375, 275}, {0, 30});

        static const Gui::DropdownList cullModeList = {
            {static_cast<uint32_t>(RasterizerState::CullMode::Front), "Front"},
            {static_cast<uint32_t>(RasterizerState::CullMode::Back), "Back"},
            {static_cast<uint32_t>(RasterizerState::CullMode::None), "None"}
        };

        if (window.dropdown("Cull mode", cullModeList, reinterpret_cast<uint32_t&>(mSettings.cullMode)))
            applyRasterStateSettings();

        static const Gui::DropdownList fillModeList = {
            {static_cast<uint32_t>(RasterizerState::FillMode::Solid), "Solid"},
            {static_cast<uint32_t>(RasterizerState::FillMode::Wireframe), "Wireframe"}
        };

        if (window.dropdown("Fill mode", fillModeList, reinterpret_cast<uint32_t&>(mSettings.fillMode)))
            applyRasterStateSettings();

        if (window.button("Load model"))
        {
            loadModel();
        }

        window.checkbox("Use custom model loader", mSettings.useCustomLoader);
        window.checkbox("Show fps", mSettings.showFPS);

        if (auto lightGroup = window.group("Point light settings"))
        {
            window.rgbColor("light ambient", mSettings.lightSettings.ambient);
            window.rgbColor("light diffuse", mSettings.lightSettings.diffuse);
            window.rgbColor("light specular", mSettings.lightSettings.specular);
            window.var("light direction", mSettings.lightSettings.lightDir);
        }

        if (auto lightGroup = window.group("Model settings"))
        {
            window.rgbColor("material ambient", mSettings.materialSettings.ambient);
            window.rgbColor("material diffuse", mSettings.materialSettings.diffuse);
            window.rgbColor("material specular", mSettings.materialSettings.specular);
        }

    }

    void ModelLoader::loadModel()
    {
        mReadyToDraw = false;
        std::filesystem::path path;

        if (openFileDialog({{"obj", "obj file"}}, path))
        {
            if (mSettings.useCustomLoader)
                loadModelFromObj(path);
            else
                loadModelFalcor(path);
        }

        const Vao::SharedPtr pVao = createVao();
        if (pVao != nullptr)
        {
            mReadyToDraw = true;
            mpGraphicsState->setVao(pVao);
            mFrameRate.reset();
        }
    }

    void ModelLoader::loadModelFalcor(const std::filesystem::path& path)
    {
        mpModel = TriangleMesh::createFromFile(path);
    }

    void ModelLoader::loadModelFromObj(const std::filesystem::path& path)
    {
        mpModel = MeshLoader::loadMeshFromObjFile(path);
    }

    void ModelLoader::applyRasterStateSettings() const
    {
        if (mpGraphicsState == nullptr)
            return;

        RasterizerState::Desc rsDesc;
        rsDesc.setCullMode(mSettings.cullMode);
        rsDesc.setFillMode(mSettings.fillMode);
        mpGraphicsState->setRasterizerState(RasterizerState::create(rsDesc));
    }

    Vao::SharedPtr ModelLoader::createVao() const
    {
        if (mpModel == nullptr)
            return nullptr;

        if (mpModel->getVertices().empty() || mpModel->getIndices().empty())
            return nullptr;

        const ResourceBindFlags ibBindFlags = Resource::BindFlags::Index | ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess;
        const Buffer::SharedPtr pIndexBuffer = Buffer::createStructured(
            mpDevice.get(),
            sizeof(uint32_t),
            mpModel->getIndices().size(),
            ibBindFlags, Buffer::CpuAccess::None,
            mpModel->getIndices().data()
        );

        const ResourceBindFlags vbBindFlags = Resource::BindFlags::Vertex | ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess;
        const Buffer::SharedPtr pVertexBuffer = Buffer::createStructured(
            mpDevice.get(),
            sizeof(TriangleMesh::Vertex),
            mpModel->getVertices().size(),
            vbBindFlags, Buffer::CpuAccess::None,
            mpModel->getVertices().data()
        );

        const VertexLayout::SharedPtr pLayout = VertexLayout::create();
        const VertexBufferLayout::SharedPtr pBufLayout = VertexBufferLayout::create();
        pBufLayout->addElement("POSOBJ", offsetof(TriangleMesh::Vertex, position), ResourceFormat::RGB32Float, 1, 0);
        pBufLayout->addElement("NORMAL", offsetof(TriangleMesh::Vertex, normal), ResourceFormat::RGB32Float, 1, 1);
        pBufLayout->addElement("TEXCOORD", offsetof(TriangleMesh::Vertex, texCoord), ResourceFormat::RG32Float, 1, 2);
        pLayout->addBufferLayout(0, pBufLayout);

        const Vao::BufferVec buffers{ pVertexBuffer };
        Vao::SharedPtr pVao = Vao::create(Vao::Topology::TriangleList, pLayout, buffers, pIndexBuffer, ResourceFormat::R32Uint);

        return pVao;
    }
}

int main()
{
    Falcor::SampleAppConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.windowDesc.title = "Model Loader";

    Falcor::Tutorial::ModelLoader ModelLoader(config);
    return ModelLoader.run();
}
