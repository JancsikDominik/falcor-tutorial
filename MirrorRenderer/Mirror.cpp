#include "Mirror.h"

namespace Falcor::Tutorial
{

    Object::Object(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name)
        : mpMesh {std::move(mesh)}, mpTexture {nullptr}, mpVao {nullptr}, mName {name}, mpDevice {device}
    {
        if (!createVao())
            mpVao = nullptr;
    }

    void Object::setTransform(Transform transform)
    {
        mSettings.pos = transform.getTranslation();
        mSettings.rotation = transform.getRotationEuler();
        mSettings.scale = transform.getScaling();
        mTransform = std::move(transform);
    }

    void Object::setTexture(Texture::SharedPtr texture)
    {
        mpTexture = std::move(texture);
    }

    Transform Object::getTransform() const
    {
        return mTransform;
    }

    Texture::SharedPtr Object::getTexture() const
    {
        return mpTexture;
    }

    Vao::SharedPtr Object::getVao() const
    {
        return mpVao;
    }

    uint32_t Object::getIndexCount() const
    {
        if (mpMesh != nullptr)
            return mpMesh->getIndices().size();

        return 0;
    }

    Object::Settings Object::getSettings() const
    {
        return mSettings;
    }

    void Object::onGuiRender(Gui::Window& window)
    {
        if (auto modelGroup = window.group(mName))
        {
            window.rgbColor((mName + " ambient").c_str(), mSettings.ambient);
            window.rgbColor((mName + " diffuse").c_str(), mSettings.diffuse);
            window.rgbColor((mName + " specular").c_str(), mSettings.specular);

            window.separator();

            bool transformChanged = false;

            if (window.var((mName + " position").c_str(), mSettings.pos))
                transformChanged = true;
            if (window.var((mName + " scale").c_str(), mSettings.scale))
                transformChanged = true;
            if (window.var((mName + " rotation (radian)").c_str(), mSettings.rotation))
                transformChanged = true;

            window.separator();

            if (window.button(("Upload texture for " + mName).c_str()))
            {
                std::filesystem::path path;
                if (openFileDialog({{"png", ""}, {"jpg", ""}}, path))
                {
                    mpTexture = Texture::createFromFile(mpDevice, path, true, false);
                }
            }

            if (transformChanged)
            {
                Transform newTransform;

                newTransform.setScaling(mSettings.scale);
                newTransform.setTranslation(mSettings.pos);
                newTransform.setRotationEuler(mSettings.rotation);

                mTransform = newTransform;
            }
        }
    }

    bool Object::createVao()
    {
        if (mpMesh == nullptr)
            return false;

        if (mpMesh->getVertices().empty() || mpMesh->getIndices().empty())
            return false;

        const ResourceBindFlags ibBindFlags = Resource::BindFlags::Index | ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess;
        const Buffer::SharedPtr pIndexBuffer = Buffer::createStructured(
            mpDevice,
            sizeof(uint32_t),
            mpMesh->getIndices().size(),
            ibBindFlags, Buffer::CpuAccess::None,
            mpMesh->getIndices().data()
        );

        const ResourceBindFlags vbBindFlags = Resource::BindFlags::Vertex | ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess;
        const Buffer::SharedPtr pVertexBuffer = Buffer::createStructured(
            mpDevice,
            sizeof(TriangleMesh::Vertex),
            mpMesh->getVertices().size(),
            vbBindFlags, Buffer::CpuAccess::None,
            mpMesh->getVertices().data()
        );

        const VertexLayout::SharedPtr pLayout = VertexLayout::create();
        const VertexBufferLayout::SharedPtr pBufLayout = VertexBufferLayout::create();
        pBufLayout->addElement("POSOBJ", offsetof(TriangleMesh::Vertex, position), ResourceFormat::RGB32Float, 1, 0);
        pBufLayout->addElement("NORMAL", offsetof(TriangleMesh::Vertex, normal), ResourceFormat::RGB32Float, 1, 1);
        pBufLayout->addElement("TEXCOORD", offsetof(TriangleMesh::Vertex, texCoord), ResourceFormat::RG32Float, 1, 2);
        pLayout->addBufferLayout(0, pBufLayout);

        const Vao::BufferVec buffers{pVertexBuffer};
        mpVao = Vao::create(Vao::Topology::TriangleList, pLayout, buffers, pIndexBuffer, ResourceFormat::R32Uint);

        return true;
    }

    RenderToTextureMirror::RenderToTextureMirror(const float2& size, Device* device, const std::string_view name)
        : Object(TriangleMesh::createQuad(size), device, name), mpCamera(Camera::create("mirror camera"))
    {
        Fbo::Desc fboDesc;
        fboDesc.setColorTarget(0, ResourceFormat::RGBA32Float);
        mpFbo = Fbo::create2D(mpDevice, mTextureResolution * size.x, mTextureResolution * size.y, fboDesc);

        mpTexture = mpFbo->getColorTexture(0);

        mpCamera->setPosition({0, 0, 0});
        mpCamera->setTarget({0.022, 0.997, 0.071});
        mpCamera->setDepthRange(0.1f, 1000.f);
        mpCamera->setFocalLength(30.f);
        mpCamera->setAspectRatio((mTextureResolution * size.x) / (mTextureResolution * size.y));

        mSettings.ambient = {1, 1, 1};
        mSettings.diffuse = {0, 0, 0};
        mSettings.specular = {0, 0, 0};
    }

    void RenderToTextureMirror::setTexture(Texture::SharedPtr texture)
    {
        // disable manual texture setting
        throw Exception("can't set texture manually for a render to texture mirror");
    }

    void RenderToTextureMirror::setTransform(Transform transform)
    {
        Object::setTransform(transform);
    }

    void RenderToTextureMirror::clearTexture()
    {
        const Texture::BindFlags flags = Texture::BindFlags::ShaderResource | Texture::BindFlags::RenderTarget;

        mpTexture = Texture::create2D(
            mpDevice, mpFbo->getWidth(), mpFbo->getHeight(), mpFbo->getDesc().getColorTargetFormat(0), 1, Resource::kMaxPossible, nullptr, flags
        );

        mpFbo->attachColorTarget(mpTexture, 0, 0, 0, Fbo::kAttachEntireMipLevel);
        mpTexture = mpFbo->getColorTexture(0);
    }
}
