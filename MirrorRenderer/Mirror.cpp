#include "Mirror.h"

#include "Core/API/RenderContext.h"

namespace Falcor::Tutorial
{
    RenderToTextureMirror::RenderToTextureMirror(const float2& size, Device* device, const std::string_view name)
        : Object(TriangleMesh::createQuad(size), device, name), mpCamera(Camera::create("mirror camera")), mQuadSize(size)
    {
        Fbo::Desc fboDesc;
        fboDesc.setColorTarget(0, ResourceFormat::RGBA32Float);
        mpFbo = Fbo::create2D(mpDevice, size.x * mTextureResolution, size.y * mTextureResolution, fboDesc);

        mpTexture = mpFbo->getColorTexture(0);

        mpCamera->setDepthRange(.1f, 200.f);
        mpCamera->setFocalLength(22.75f);
        mpCamera->setAspectRatio((size.x * mTextureResolution) / (size.y * mTextureResolution));
        mpCamera->setUpVector({0, 1, 0});

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

        /*Transform cameraRotation;
        float3 rotationAngles = transform.getRotationEuler();
        rotationAngles = -rotationAngles;
        cameraRotation.setRotationEuler(transform.getRotationEuler());

        mpCamera->setUpVector(float3(0, 0, 1) * rmcv::mat3(cameraRotation.getMatrix()));*/

        const auto& inverseTranspose4by4 = transpose(inverse(transform.getMatrix()));
        const rmcv::mat3& inverseTranspose = inverseTranspose4by4;

        const float3& surfaceNormal = inverseTranspose * float3(0, 1, 0);
        mSurfaceNormal = normalize(surfaceNormal);

        mpCamera->setAspectRatio((transform.getScaling().x * mQuadSize.x )/ (transform.getScaling().z * mQuadSize.y));

        setViewAngle(mObserverPos);
        Object::setTransform(transform);
    }

    void RenderToTextureMirror::onGuiRender(Gui::Window& window)
    {
        if (auto modelGroup = window.group(mName))
        {
            window.text(
                "surface normal: " + std::to_string(mSurfaceNormal.x) + " " +
                std::to_string(mSurfaceNormal.y) + "  " +
                std::to_string(mSurfaceNormal.z)
            );

            window.text(
                "reflection vector: " +
                std::to_string(mReflectionVector.x) + " " +
                std::to_string(mReflectionVector.y) + " " +
                std::to_string(mReflectionVector.z)
            );

            window.separator();

            mpCamera->renderUI(window);
        }
    }

    void RenderToTextureMirror::setViewAngle(const float3& observerPos)
    {
        float3 inVector = normalize(mpCamera->getPosition() - observerPos);
        mReflectionVector = inVector - 2 * dot(inVector, mSurfaceNormal) * mSurfaceNormal;
        mpCamera->setTarget(mReflectionVector);
        mObserverPos = observerPos;
    }

    void RenderToTextureMirror::clearMirror(RenderContext* context, const float4& clearCorlor) const
    {
        context->clearFbo(mpFbo.get(), clearCorlor, 1.0f, 0, FboAttachmentType::All);
    }
}
