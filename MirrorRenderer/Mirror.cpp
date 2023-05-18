#include "Mirror.h"

#include "Core/API/RenderContext.h"

namespace Falcor::Tutorial
{
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

        mSurfaceNormal = {0, 1, 0};
    }

    void RenderToTextureMirror::setTexture(Texture::SharedPtr texture)
    {
        // disable manual texture setting
        throw Exception("can't set texture manually for a render to texture mirror");
    }

    void RenderToTextureMirror::setTransform(Transform transform)
    {
        mpCamera->setPosition(transform.getTranslation());
        mSurfaceNormal = (float4(mSurfaceNormal, 1) * transform.getMatrix()).xyz;

        Object::setTransform(transform);

        // mpCamera->setTarget(mpCamera->getTarget() * mSettings.rotation);
    }

    void RenderToTextureMirror::onGuiRender(Gui::Window& window)
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

                setTransform(newTransform);
            }
        }
    }

    void RenderToTextureMirror::setViewAngle(const float3& observerPos) const
    {
        float3 inVector = normalize(mpCamera->getPosition() - observerPos);
        float3 reflectionVector = inVector - 2 * dot(inVector, mSurfaceNormal) * mSurfaceNormal;
        mpCamera->setTarget(reflectionVector);
    }

    void RenderToTextureMirror::clearMirror(RenderContext* context, const float4& clearCorlor) const
    {
        context->clearFbo(mpFbo.get(), clearCorlor, 1.0f, 0, FboAttachmentType::All);
    }
}
