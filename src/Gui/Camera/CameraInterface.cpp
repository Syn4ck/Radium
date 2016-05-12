#include <Gui/Camera/CameraInterface.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{
    namespace Gui
    {
        CameraInterface::CameraInterface(uint width, uint height)
            : m_cameraSensitivity(1.0)
            , m_hasLightAttached(false)
        {
            m_camera.reset(new Engine::Camera(Scalar(height), Scalar(width)));

            setCameraFovInDegrees(60.0);
            setCameraZNear(0.1);
            setCameraZFar(1000.0);
        }

        CameraInterface::~CameraInterface()
        {
        }

        void CameraInterface::resizeViewport(uint width, uint height)
        {
            m_camera->resize(Scalar(width), Scalar(height));
        }

        Core::Matrix4 CameraInterface::getProjMatrix() const
        {
            return m_camera->getProjMatrix();
        }

        Core::Matrix4 CameraInterface::getViewMatrix() const
        {
            return m_camera->getViewMatrix();
        }

        void CameraInterface::setCameraSensitivity(double sensitivity)
        {
            m_cameraSensitivity = sensitivity;
        }

        void CameraInterface::setCameraFov(double fov)
        {
            m_camera->setFOV(fov);
        }

        void CameraInterface::setCameraFovInDegrees(double fov)
        {
            m_camera->setFOV(fov * Core::Math::toRad);
        }

        void CameraInterface::setCameraZNear(double zNear)
        {
            m_camera->setZNear(zNear);
        }

        void CameraInterface::setCameraZFar(double zFar)
        {
            m_camera->setZFar(zFar);
        }
    } // namespace Gui
} // namespace Ra
