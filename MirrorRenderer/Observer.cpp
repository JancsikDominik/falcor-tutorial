#include "Observer.h"


namespace Falcor::Tutorial
{
    // 
    // Observer 
    //

    Observer::Observer(TriangleMesh::SharedPtr mesh, Device* device, const std::string_view name)
        : Object(mesh, device, name), mpCamera(Camera::create())
    {
        mpCamera->setDepthRange(0.5f, 100);
    }

    void Observer::setTransform(Transform transform)
    {
        mpCamera->setPosition(transform.getTranslation());
        Object::setTransform(transform);
    }

    float3 Observer::getCameraRotation() const
    {
        auto viewMatrix = mpCamera->getViewMatrix();

        // getting scale
        float3 scale;
        scale.x = length(float3(viewMatrix[0][0], viewMatrix[0][1], viewMatrix[0][2]));
        scale.y = length(float3(viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2]));
        scale.z = length(float3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]));

        // removing scale
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                viewMatrix[i][j] = viewMatrix[i][j] / scale[j];
            }
        }

        // removing transation
        viewMatrix[0][3] = 0;
        viewMatrix[1][3] = 0;
        viewMatrix[2][3] = 0;

        // getting euler angles from rotation matrix
        float sy = sqrt(viewMatrix[0][0] * viewMatrix[0][0] + viewMatrix[1][0] * viewMatrix[1][0]);
        bool singular = sy < 1e-6;
        float x, y, z;
        if (!singular)
        {
            x = atan2(viewMatrix[1][0], viewMatrix[0][0]);
            y = atan2(-viewMatrix[2][0], sy);
            z = atan2(viewMatrix[2][1], viewMatrix[2][2]);
        }
        else
        {
            x = 0;
            y = atan2(-viewMatrix[2][0], sy);
            z = atan2(-viewMatrix[1][2], viewMatrix[1][1]);
        }

        return {-z, y, x};
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
            t.setTranslation(mpCamera->getPosition());
            t.setRotation(getCameraRotation());
            t.setScaling(mSettings.scale);
            setTransform(t);
            return true;
        }

        return false;
    }
}
