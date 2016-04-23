#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP

#include <Gui/RaGuiBase.hpp>

#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra
{
    namespace Engine
    {
        class Camera;
        class Light;
    }
}

namespace Ra
{
    namespace Gui
    {

        class RA_GUIBASE_API CameraInterface
        {
        public:
            // FIXME(Charly): width / height ?
            CameraInterface(uint width, uint height);
            virtual ~CameraInterface();

            void resizeViewport(uint width, uint height);

            Core::Matrix4 getProjMatrix() const;
            Core::Matrix4 getViewMatrix() const;

            virtual void handleEvents() = 0;

            const Engine::Camera* getCamera() const
            {
                return m_camera.get();
            }

            void attachLight(const std::shared_ptr<Engine::Light>& light)
            {
                m_light            = light;
                m_hasLightAttached = true;
            }

            virtual void update(Scalar dt) = 0;

            void setCameraSensitivity(double sensitivity);

            void setCameraFov(double fov);
            void setCameraFovInDegrees(double fov);
            void setCameraZNear(double zNear);
            void setCameraZFar(double zFar);

            virtual void fitScene(const Core::Aabb& aabb) = 0;

            virtual void setCameraPosition(const Core::Vector3& position) = 0;
            virtual void setCameraTarget(const Core::Vector3& target) = 0;

            virtual void resetCamera() = 0;

        protected:
            Scalar m_cameraSensitivity;

            std::unique_ptr<Engine::Camera> m_camera;

            std::shared_ptr<Engine::Light> m_light;
            bool m_hasLightAttached;
        };

    } // namespace Ra
} // namespace Engine

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP
