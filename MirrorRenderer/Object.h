#pragma once

#include "Scene/TriangleMesh.h"
#include "Core/API/VAO.h"
#include "Core/API/Texture.h"
#include "Utils/UI/Gui.h"

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

        using SharedPtr = std::shared_ptr<Object>;
        using List = std::vector<Object::SharedPtr>;

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

        virtual void onGuiRender(Gui::Window& window);

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
}
