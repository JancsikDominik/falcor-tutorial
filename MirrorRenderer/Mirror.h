#pragma once
#include "Object.h"

#include "Scene/Camera/Camera.h"

namespace Falcor::Tutorial
{
    class RenderToTextureMirror final : public Object
    {
    public:
        // mirror can only be a quad
        RenderToTextureMirror(const float2& size, Device* device, std::string_view name);

        void setTexture(Texture::SharedPtr texture) override;
        void setTransform(Transform transform) override;
        void onGuiRender(Gui::Window& window) override;
        FlipTextureAxis getTextureFlipAxis() override { return Y; }

        void setViewAngle(const float3& observerPos);
        void clearMirror(RenderContext* context, const float4& clearCorlor) const;

        Fbo::SharedPtr getFbo() const { return mpFbo; }
        const float3& getCameraPos() const { return mSettings.pos; }
        rmcv::mat4 getCameraViewProjMatrix() const { return mpCamera->getViewProjMatrix(); }

    private:
        Camera::SharedPtr mpCamera;
        Fbo::SharedPtr mpFbo;
        float3 mObserverPos;
        float3 mReflectionVector;
        float2 mQuadSize;

        rmcv::mat4 mViewMatrix;

        float3 mSurfaceNormal;

        uint32_t mTextureResolution = 512;
    };
}
