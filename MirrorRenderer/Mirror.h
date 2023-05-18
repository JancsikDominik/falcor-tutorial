#pragma once
#include "Object.h"

#include "Scene/Camera/Camera.h"

namespace Falcor::Tutorial
{
    class RenderToTextureMirror final : public Object
    {
    public:
        // mirror can only be a quad
        RenderToTextureMirror(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name) = delete;
        RenderToTextureMirror(const float2& size, Device* device, std::string_view name);

        void setTexture(Texture::SharedPtr texture) override;
        void setTransform(Transform transform) override;
        void onGuiRender(Gui::Window& window) override;
        void setViewAngle(const float3& observerPos) const;
        void clearMirror(RenderContext* context, const float4& clearCorlor) const;

        Fbo::SharedPtr getFbo() const { return mpFbo; }
        const Camera& getCamera() const { return *mpCamera; }

    private:
        Camera::SharedPtr mpCamera;
        Fbo::SharedPtr mpFbo;

        float3 mSurfaceNormal;

        uint32_t mTextureResolution = 512;
    };
}
