#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP

#include <Gui/Camera/CameraInterface.hpp>

namespace Ra
{
    namespace Gui
    {
        class RA_GUIBASE_API TrackballCamera : public CameraInterface
        {
        public:
            TrackballCamera(uint width, uint height);
            virtual ~TrackballCamera();

            virtual void handleEvents() override;

            virtual void update(Scalar dt) override;

            virtual void setCameraPosition(const Core::Vector3& position) override;
            virtual void setCameraTarget(const Core::Vector3& target) override;
            virtual void fitScene(const Core::Aabb& aabb) override;

            virtual void resetCamera() override;

        private:
            bool handleMousePressEvent(const Ra::Core::MouseEvent* event);
            bool handleMouseMoveEvent(const Ra::Core::MouseEvent* event);
            bool handleMouseReleaseEvent(const Ra::Core::MouseEvent* event);
            bool handleWheelEvent(const Ra::Core::MouseEvent* event);

            bool handleKeyPressEvent(const Ra::Core::KeyEvent* event);
            bool handleKeyReleaseEvent(const Ra::Core::KeyEvent* event);

            void handleCameraRotate(Scalar dx, Scalar dy);
            void handleCameraPan(Scalar dx, Scalar dy);
            void handleCameraZoom(Scalar dx, Scalar dy);
            void handleCameraZoom(Scalar z);

            void updatePhiTheta();

        private:
            Core::Vector3 m_trackballCenter;
            Core::Vector2 m_lastMousePos;

            Scalar m_quickCameraModifier;
            Scalar m_wheelSpeedModifier;

            Scalar m_phi;
            Scalar m_theta;

            Scalar m_distFromCenter;
            Scalar m_cameraRadius;

            bool m_rotateAround;
            bool m_cameraRotateMode;
            bool m_cameraPanMode;
            // TODO(Charly): fps mode
            bool m_cameraZoomMode;

            bool m_walkingOn;
            bool m_strafingOn;
            bool m_climbingOn;

            Scalar m_walking;
            Scalar m_strafing;
            Scalar m_climbing;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
