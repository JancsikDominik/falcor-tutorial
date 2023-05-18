#include "Observer.h"


namespace Falcor::Tutorial
{
    // 
    // Observer 
    //

    Observer::Observer(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name)
        : Object(mesh, device, name), mpCamera(Camera::create())
    {
    }

    //
    // FpsObserver
    //

    FpsObserver::FpsObserver(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name)
        : Observer(mesh, device, name), mpCameraController(FirstPersonCameraController::create(mpCamera))
    {
    }

    bool FpsObserver::onMouseEvent(const MouseEvent& mouseEvent)
    {
        return mpCameraController->onMouseEvent(mouseEvent);
    }

    bool FpsObserver::onKeyEvent(const KeyboardEvent& keyboardEvent)
    {
        return mpCameraController->onKeyEvent(keyboardEvent);
    }

    bool FpsObserver::update()
    {
        if(mpCameraController->update())
        {
            Transform t;
            float3 camPos = mpCamera->getPosition();
            t.setTranslation(camPos);
            mSettings.pos = camPos;
            mSettings.rotation = float3(0, 0, 0);
            mpMesh->applyTransform(t);
            return true;
        }

        return false;
    }
}
