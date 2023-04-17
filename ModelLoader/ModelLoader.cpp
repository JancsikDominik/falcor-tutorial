#include "ModelLoader.h"

// from Falcor
#include "Utils/UI/TextRenderer.h"

#include <fstream>


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
    }

    void ModelLoader::onLoad(RenderContext* pRenderContext)
    {
        // TODO: move load model to ui
        loadModel("C:/Users/Jancsik/Documents/cube.obj");
    }

    void ModelLoader::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        mpCameraController->update();
        pRenderContext->clearFbo(pTargetFbo.get(), {0, 0.25, 0, 1}, 1.0f, 0, FboAttachmentType::All);
        mpVars["VSCBuffer"]["model"] = float4x4(1); // identity matrix
        mpVars["VSCBuffer"]["viewProjection"] = mpCamera->getViewProjMatrix();

        mpGraphicsState->setFbo(pTargetFbo);

        if (mReadyToDraw)
            pRenderContext->drawIndexed(mpGraphicsState.get(), mpVars.get(), mpModel->getIndices().size(), 0, 0);
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
        Gui::Window window(pGui, "Settings", {375, 275}, {5, 5});

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

        if (window.button("Use custom model loader (only works for .obj files)"))
        {
            mSettings.useCustomLoader = !mSettings.useCustomLoader;
        }

        if (window.button("Show fps"))
        {
            mSettings.showFPS = !mSettings.showFPS;
        }

    }

    void ModelLoader::loadModel(const std::filesystem::path& path)
    {
        mReadyToDraw = false;
        if (mSettings.useCustomLoader)
            loadModelFromObj(path);
        else
            loadModelFalcor(path);

        const Vao::SharedPtr pVao = createVao();
        if (pVao != nullptr)
        {
            mReadyToDraw = true;
            mpGraphicsState->setVao(pVao);
        }
    }

    void ModelLoader::loadModelFalcor(const std::filesystem::path& path)
    {
        mpModel = TriangleMesh::createFromFile(path);
    }

    void ModelLoader::loadModelFromObj(const std::filesystem::path& path)
    {
        if (path.extension().string() != ".obj")
            throw std::exception("Unsupported file type.");

        TriangleMesh::VertexList vertices;
        TriangleMesh::IndexList  indices;

        /*
         * loading data from obj file manually:
         * .obj file structure:
         *   '#' is a comment, just like '//' in C++, so we can skip those lines
         *   'v' is a vertex
         *   'vt' is the texture coordinate of one vertex, we skip these lines for now, we aren't applying textures in this tutorial
         *   'vn' is the normal of one vertex, we skip these lines for now, we aren't applying textures in this tutorial
         *   'f' is a face
         *   for f a/b/c d/e/f g/h/i :
         *       a/b/c describes the first vertex of the triangle
         *       d/e/f describes the second vertex of the triangle
         *       g/h/i describes the third vertex of the triangle
         *   for the first vertex:
         *      'a' says which vertex to use.
         *      'b' says which texture coordinate to use.
         *      'c' says which normal to use.
         *      'a', 'b' and 'c' are indexed from 1 not from 0 like in c++
         */

        std::string line;
        std::ifstream inFile(path);

        if (!inFile)
            return;

        while (std::getline(inFile, line))
        {
            TriangleMesh::Vertex v;

            std::stringstream ss(line);
            std::string inType;

            ss >> inType;

            if (inType == "v")
            {
                ss >> v.position.x >> v.position.y >> v.position.z;
                vertices.push_back(v);
            }
            else if (inType == "vt")
            {
                ss >> v.texCoord.x >> v.texCoord.y;
                vertices.push_back(v);
            }
            else if (inType == "vn")
            {
                ss >> v.normal.x >> v.normal.y >> v.normal.z;
                vertices.push_back(v);
            }
            else if (inType == "f")
            {
                std::string vertexIndices;

                while (std::getline(ss, vertexIndices, ' '))
                {
                    if (vertexIndices.empty())
                        continue;

                    std::istringstream indexSS(vertexIndices);
                    std::string token;

                    // position index
                    std::getline(indexSS, token, '/');
                    indices.push_back(std::stoi(token) - 1);

                    // texCoord index
                    std::getline(indexSS, token, '/');
                    indices.push_back(std::stoi(token) - 1);

                    // normal index
                    std::getline(indexSS, token, '/');
                    indices.push_back(std::stoi(token) - 1);
                }
            }
        }

        // TODO: create faces from indices and vertices

        mpModel = TriangleMesh::create(vertices, indices);
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
