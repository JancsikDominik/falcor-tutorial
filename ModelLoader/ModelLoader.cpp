#include "ModelLoader.h"

// from Falcor
#include "Utils/UI/TextRenderer.h"

// from std
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
        mpState = GraphicsState::create(mpDevice);
        mpState->setProgram(mpProgram);

        mpVars = GraphicsVars::create(mpDevice, mpProgram->getReflector());

        RasterizerState::Desc rsDesc;
        rsDesc.setFillMode(RasterizerState::FillMode::Solid).setCullMode(RasterizerState::CullMode::None).setScissorTest(true).setDepthClamp(false);
        mpState->setRasterizerState(RasterizerState::create(rsDesc));

        DepthStencilState::Desc dsDesc;
        dsDesc.setDepthEnabled(false);
        mpState->setDepthStencilState(DepthStencilState::create(dsDesc));
        // mpCamera = Camera::create("main camera");
    }

    void ModelLoader::onLoad(RenderContext* pRenderContext)
    {
        // TODO: dont do this xd
        loadModel("C:/Users/Jancsik/Documents/suzanne.obj");
    }

    void ModelLoader::onFrameRender(RenderContext* pRenderContext, const Fbo::SharedPtr& pTargetFbo)
    {
        pRenderContext->clearFbo(pTargetFbo.get(), {0, 0, 0, 1}, 1.0f, 0, FboAttachmentType::All);

        // mpVars["VSCBuffer"]["viewProjection"] = mpCamera->getViewProjMatrix();
        mpState->setFbo(pTargetFbo, false);
        mpState->setVao(mpVao);

        pRenderContext->draw(mpState.get(), mpVars.get(), mpModel->getVertices().size(), 0);
    }

    void ModelLoader::onResize(uint32_t width, uint32_t height)
    {
        const float h = static_cast<float>(height);
        const float w = static_cast<float>(width);

        if (mpCamera != nullptr)
        {
            mpCamera->setFocalLength(18);
            const float aspectRatio = (w / h);
            mpCamera->setAspectRatio(aspectRatio);
        }
    }

    bool ModelLoader::onKeyEvent(const KeyboardEvent& keyEvent)
    {
        // TODO: camera controls
        return true;
    }

    bool ModelLoader::onMouseEvent(const MouseEvent& mouseEvent)
    {
        // TODO: camera controls
        return true;
    }

    void ModelLoader::onGuiRender(Gui* pGui)
    {
        // TODO: switch between loading methods (my loading vs. built in)
    }

    void ModelLoader::loadModel(const std::filesystem::path& path)
    {
        if (mSettings.useCustomLoader)
            loadModelFromObj(path);
        else
            loadModelFalcor(path);

        Buffer::SharedPtr pIndexBuffer;
        const ResourceBindFlags ibBindFlags = Resource::BindFlags::Index | ResourceBindFlags::ShaderResource;
        pIndexBuffer = Buffer::create(
            mpDevice.get(),
            sizeof(uint32_t) * mpModel->getIndices().size(),
            ibBindFlags,
            Buffer::CpuAccess::None,
            mpModel->getIndices().data()
        );

        const ResourceBindFlags vbBindFlags = Resource::BindFlags::Vertex | ResourceBindFlags::ShaderResource;
        mpVertexBuffer = Buffer::createStructured(
            mpDevice.get(), sizeof(TriangleMesh::Vertex),
            mpModel->getVertices().size(), vbBindFlags,
            Buffer::CpuAccess::None,
            mpModel->getVertices().data()
        );

        const VertexLayout::SharedPtr pLayout = VertexLayout::create();
        const VertexBufferLayout::SharedPtr pBufLayout = VertexBufferLayout::create();
        pBufLayout->addElement("SV_POSITION", offsetof(TriangleMesh::Vertex, position), ResourceFormat::RG32Float, 1, 0);
        pLayout->addBufferLayout(0, pBufLayout);

        const Vao::BufferVec buffers{ mpVertexBuffer };
        mpVao = Vao::create(Vao::Topology::TriangleList, pLayout, buffers, pIndexBuffer, ResourceFormat::R32Uint);
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
            else if (inType == "f")
            {
                std::string vertexIndices;

                while (std::getline(ss, vertexIndices, ' '))
                {
                    std::string vertexPositionIndex;
                    while (std::getline(ss, vertexPositionIndex, '/'))
                    {
                        indices.push_back(std::stoi(vertexPositionIndex));
                        // not using textures, or normals
                        break;
                    }
                }
            }
        }

        mpModel = TriangleMesh::create(vertices, indices);
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
