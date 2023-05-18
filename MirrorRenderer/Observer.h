#pragma once

#include "Object.h"
#include "Scene/Camera/Camera.h"
#include "Scene/Camera/CameraController.h"

namespace Falcor::Tutorial
{
    class Observer : public Object
    {
    public:
        Observer(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name);

        using SharedPtr = std::shared_ptr<Observer>;

        virtual bool onMouseEvent(const MouseEvent& mouseEvent) = 0;
        virtual bool onKeyEvent(const KeyboardEvent& keyboardEvent) = 0;
        virtual bool update() = 0;

    protected:
        Camera::SharedPtr mpCamera;
    };

    class FpsObserver final : public Observer
    {
    public:
        FpsObserver(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name);

        bool onMouseEvent(const MouseEvent& mouseEvent) override;
        bool onKeyEvent(const KeyboardEvent& keyboardEvent) override;
        bool update() override;

    private:
        FirstPersonCameraController::SharedPtr mpCameraController;
    };

}
