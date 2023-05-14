#pragma once
#include "Core/SampleApp.h"
#include "RenderGraph/BasePasses/FullScreenPass.h"
#include "Scene/TriangleMesh.h"

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

        Object(TriangleMesh::SharedPtr mesh, Device* device, std::string_view name);

        void setTransform(Transform transform);
        void setTexture(Texture::SharedPtr texture);

        Transform getTransform() const;
        Texture::SharedPtr getTexture() const;
        Vao::SharedPtr getVao() const;
        uint32_t getIndexCount() const;
        Settings getSettings() const;

        void onGuiRender(Gui::Window& window);

    private:
        bool createVao();

        TriangleMesh::SharedPtr mpMesh;
        Texture::SharedPtr mpTexture;
        Transform mTransform;
        Vao::SharedPtr mpVao;
        std::string mName;
        Device* mpDevice;

        Settings mSettings;
    };

    class RenderToTextureMirror : public Object
    {
        // TODO: maybe
    };
}
