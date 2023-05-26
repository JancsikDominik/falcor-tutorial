#include "MirrorRenderer.h"

#include "Utils/UI/TextRenderer.h"


namespace Falcor::Tutorial
{
    bool MirrorRenderer::RenderSettings::onGuiRender(Gui::Window& window)
    {
        bool hasSettingsChanged = false;

        static const Gui::DropdownList cullModeList = {
            {static_cast<uint32_t>(RasterizerState::CullMode::Front), "Front"},
            {static_cast<uint32_t>(RasterizerState::CullMode::Back), "Back"},
            {static_cast<uint32_t>(RasterizerState::CullMode::None), "None"}};

        if (window.dropdown("Cull mode", cullModeList, reinterpret_cast<uint32_t&>(cullMode)))
            hasSettingsChanged = true;

        static const Gui::DropdownList fillModeList = {
            {static_cast<uint32_t>(RasterizerState::FillMode::Solid), "Solid"},
            {static_cast<uint32_t>(RasterizerState::FillMode::Wireframe), "Wire-frame"}
        };

        if (window.dropdown("Fill mode", fillModeList, reinterpret_cast<uint32_t&>(fillMode)))
            hasSettingsChanged = true;

        window.checkbox("Show fps", showFPS);

        return hasSettingsChanged;
    }

    void MirrorRenderer::LightSettings::onGuiRender(Gui::Window& window)
    {
        if (auto lightGroup = window.group("Directional light settings"))
        {
            window.rgbColor("light ambient", ambient);
            window.rgbColor("light diffuse", diffuse);
            window.rgbColor("light specular", specular);
            window.var("light direction", lightDir);
        }
    }

    void MirrorRenderer::onLoad(RenderContext* pRenderContext)
    {
        mpDevice = getDevice();

        Program::Desc mainProgramDesc;
        mainProgramDesc.addShaderLibrary("Samples/MirrorRenderer/MirrorRenderer.vs.slang").vsEntry("main");
        mainProgramDesc.addShaderLibrary("Samples/MirrorRenderer/MirrorRenderer.ps.slang").psEntry("main");

        mpMainProgram = GraphicsProgram::create(mpDevice, mainProgramDesc);
        mpGraphicsState = GraphicsState::create(mpDevice);
        mpMainVars = GraphicsVars::create(mpDevice, mpMainProgram->getReflector());

        applyRasterStateSettings();

        DepthStencilState::Desc dsDesc;
        dsDesc.setDepthEnabled(true);
        mpGraphicsState->setDepthStencilState(DepthStencilState::create(dsDesc));

        mpTextureSampler = Sampler::create(mpDevice.get(), {});

        buildScene();

        mpObserver = std::make_shared<FpsObserver>(TriangleMesh::createFromFile("C:/Users/Jancsik/Documents/suzanne.obj"), mpDevice.get(), "Player");
        Transform observerTransform;
        observerTransform.setTranslation({0, 0, -15});
        observerTransform.setScaling({00.3, 0.3, 0.3});
        mpObserver->setTransform(observerTransform);
        mpObserver->update();
        mObjects.push_back(mpObserver);
    }

    void MirrorRenderer::onResize(uint32_t width, uint32_t height)
    {
        const float h = static_cast<float>(height);
        const float w = static_cast<float>(width);

        if (mpObserver != nullptr)
        {
            mpObserver->getCamera()->setFocalLength(30.f);
            const float aspectRatio = (w / h);
            mpObserver->getCamera()->setAspectRatio(aspectRatio);
        }
    }

    void MirrorRenderer::onFrameRender(RenderContext* pRenderContext, const std::shared_ptr<Fbo>& pTargetFbo)
    {
        mpObserver->update();
        pRenderContext->clearFbo(pTargetFbo.get(), {0, 0.25, 0, 1}, 1.0f, 0, FboAttachmentType::All);

        float3 camPos;
        rmcv::mat4 viewProj;

        if (mUpdateMirror)
        {
            const auto& mirror = dynamic_cast<RenderToTextureMirror*>(mpMirrorObj.get());
            camPos = mirror->getCameraPos();
            viewProj = mirror->getCameraViewProjMatrix();
            mirror->setViewAngle(mpObserver->getCamera()->getPosition());

            mirror->clearMirror(pRenderContext, {0, 0.25, 0, 1});
            // rendering scene onto mirror's texture
            renderObjects(pRenderContext, mirror->getFbo(), camPos, viewProj);
        }

        if (mIsMainCameraUsed || !mUpdateMirror)
        {
            camPos = mpObserver->getCamera()->getPosition();
            viewProj = mpObserver->getCamera()->getViewProjMatrix();
        }

        // removing player model
        mObjects.pop_back();

        // rendering scene normally
        renderObjects(pRenderContext, pTargetFbo, camPos, viewProj);

        // putting back player model
        mObjects.push_back(mpObserver);

        mFrameRate.newFrame();
        if (mSettings.renderSettings.showFPS)
            TextRenderer::render(pRenderContext, mFrameRate.getMsg(), pTargetFbo, {10, 10});
    }

    void MirrorRenderer::onGuiRender(Gui* pGui)
    {
        Gui::Window window(pGui, "Settings", {375, 275}, {0, 30});

        if (mSettings.renderSettings.onGuiRender(window))
            applyRasterStateSettings();

        mSettings.lightSettings.onGuiRender(window);

        for (const auto& object : mObjects)
        {
            object->onGuiRender(window);
        }

        window.checkbox("Update mirror", mUpdateMirror);

        if (window.button("switch camera"))
        {
            mIsMainCameraUsed = !mIsMainCameraUsed;
        }
    }

    bool MirrorRenderer::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        return mpObserver->onKeyEvent(keyEvent);
    }

    bool MirrorRenderer::onMouseEvent(const MouseEvent& mouseEvent)
    {
        return mpObserver->onMouseEvent(mouseEvent);
    }

    void MirrorRenderer::renderObjects(RenderContext* pRenderContext, const std::shared_ptr<Fbo>& pTargetFbo, const float3& cameraPos, const rmcv::mat4& cameraViewProjMatrix) const
    {
        mpGraphicsState->setFbo(pTargetFbo);
        mpGraphicsState->setProgram(mpMainProgram);

        mpMainVars["VSCBuffer"]["viewProjection"] = cameraViewProjMatrix;

        mpMainVars["PSCBuffer"]["cameraPosition"] = cameraPos;
        mpMainVars["PSCBuffer"]["lightAmbient"] = mSettings.lightSettings.ambient;
        mpMainVars["PSCBuffer"]["lightDiffuse"] = mSettings.lightSettings.diffuse;
        mpMainVars["PSCBuffer"]["lightSpecular"] = mSettings.lightSettings.specular;
        mpMainVars["PSCBuffer"]["lightDir"] = mSettings.lightSettings.lightDir;

        for (const auto& object : mObjects)
        {
            mpMainVars["VSCBuffer"]["model"] = object->getTransform().getMatrix();
            mpMainVars["VSCBuffer"]["modelIT"] = transpose(inverse(object->getTransform().getMatrix()));
            mpMainVars["VSCBuffer"]["flipTextureOnAxis"] = static_cast<uint32_t>(object->getTextureFlipAxis());

            mpMainVars["PSCBuffer"]["materialAmbient"] = object->getSettings().ambient;
            mpMainVars["PSCBuffer"]["materialDiffuse"] = object->getSettings().diffuse;
            mpMainVars["PSCBuffer"]["materialSpecular"] = object->getSettings().specular;
            mpMainVars["PSCBuffer"]["isMirror"] = dynamic_cast<RenderToTextureMirror*>(object.get()) != nullptr;

            const bool hasTexture = object->getTexture() != nullptr;
            mpMainVars["PSCBuffer"]["isTextureLoaded"] = hasTexture;
            if (hasTexture)
            {
                mpMainVars["PSCBuffer"]["objTexture"] = object->getTexture();
                mpMainVars["PSCBuffer"]["texSampler"] = mpTextureSampler;
            }

            mpGraphicsState->setVao(object->getVao());
            pRenderContext->drawIndexed(mpGraphicsState.get(), mpMainVars.get(), object->getIndexCount(), 0, 0);
        }
    }

    void MirrorRenderer::applyRasterStateSettings() const
    {
        if (mpGraphicsState == nullptr)
            return;

        RasterizerState::Desc rsDesc;
        rsDesc.setCullMode(mSettings.renderSettings.cullMode);
        rsDesc.setFillMode(mSettings.renderSettings.fillMode);
        mpGraphicsState->setRasterizerState(RasterizerState::create(rsDesc));
    }

    void MirrorRenderer::buildScene()
    {
        const auto floor = std::make_shared<Object>(TriangleMesh::createQuad({100, 100}), mpDevice.get(), "floor");
        Transform floorTransform;
        floorTransform.setTranslation({0, -3.5, 0});
        floor->setTransform(floorTransform);
        floor->setAmbient({1, 1, 1});
        floor->setDiffuse({1, 1, 1});
        floor->setSpecular({1, 1, 1});
        floor->setTexture(Texture::createFromFile(mpDevice.get(), "C:/Users/Jancsik/Documents/floorTexture.jpg", true, true));
        mObjects.push_back(floor);

        for (uint32_t i = 0; i < 32; i++)
        {
            const auto pMesh = i % 2 == 0 ? TriangleMesh::createCube({0.5, 0.5, 0.5}) : TriangleMesh::createSphere(0.25);
            mObjects.push_back(std::make_shared<Object>(pMesh, mpDevice.get(), "cube" + std::to_string(i)));
            Transform t;
            t.setTranslation({cos(2 * (i / M_PI)) * 3, -3 + i * (1.f / 2), sin(2 * (i / M_PI)) * 3 - 7});

            mObjects[i + 2]->setTransform(t);
            mObjects[i + 2]->setAmbient({static_cast<float>(i) * 0.01, static_cast<float>(i) * 0.03, static_cast<float>(i) * 0.04});
        }
    }
}

int main()
{
    Falcor::SampleAppConfig config;
    config.windowDesc.width = 1280;
    config.windowDesc.height = 720;
    config.windowDesc.title = "Mirror renderer";

    Falcor::Tutorial::MirrorRenderer mirrorRenderer(config);
    return mirrorRenderer.run();
}
