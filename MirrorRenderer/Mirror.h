#pragma once
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"
#include "Scene/TriangleMesh.h"
#include "Scene/Camera/Camera.h"

namespace Falcor::Tutorial
{
    class Object
    {
    public:
        struct Settings
        {
            float3 pos = {0, 0, 0};
            float3 scale = {1, 1, 1};
            float3 rotation = {0, 0, 0};

            float3 ambient = {0, 0.25, 0.2};
            float3 diffuse = {0, 0.25, 0.2};
            float3 specular = {0, 0.25, 0.2};
        };

        using List = std::vector<Object>;
        using SharedPtr = std::shared_ptr<Object>;

        Object(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name);
        virtual ~Object() = default;

        virtual void setTransform(Transform transform);
        virtual void setTexture(Texture::SharedPtr texture);

        Transform getTransform() const;
        Texture::SharedPtr getTexture() const;
        Vao::SharedPtr getVao() const;
        uint32_t getIndexCount() const;
        Settings getSettings() const;
        std::string getName() const { return mName; }

        void onGuiRender(Gui::Window& window);

    protected:
        bool createVao();

        TriangleMesh::SharedPtr mpMesh;
        Texture::SharedPtr mpTexture;
        Transform mTransform;
        Vao::SharedPtr mpVao;
        std::string mName;
        Device* mpDevice;

        Settings mSettings;
    };

    class RenderToTextureMirror final : public Object
    {
    public:
        // mirror can only be a quad
        RenderToTextureMirror(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name) = delete;
        RenderToTextureMirror(const float2& size, Device* device, std::string_view name);

        void setTexture(Texture::SharedPtr texture) override;
        // TODO:
        void setTransform(Transform transform) override;
        void clearTexture();

        Fbo::SharedPtr getFbo() const { return mpFbo; }
        const Camera& getCamera() const { return *mpCamera; }

    private:
        Camera::SharedPtr mpCamera;
        Fbo::SharedPtr mpFbo;

        uint32_t mTextureResolution = 512;
    };
}
