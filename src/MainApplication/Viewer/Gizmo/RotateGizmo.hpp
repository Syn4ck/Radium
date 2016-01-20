#ifndef RADIUMENGINE_ROTATE_GIZMO_HPP_
#define RADIUMENGINE_ROTATE_GIZMO_HPP_

#include <MainApplication/Viewer/Gizmo/Gizmo.hpp>
namespace Ra
{
    namespace Gui
    {
        /// A gizmo with 3 torus shapes to rotate around a point.
        class RotateGizmo : public Gizmo
        {
        public:
            RotateGizmo(Engine::Component* c, const Core::Transform& worldTo, const Core::Transform& t, Mode mode);

            void updateTransform(const Core::Transform &worldTo, const Core::Transform& t) override;
            void selectConstraint( int drawableIndex ) override;
            void setInitialState( const Engine::Camera& cam, const Core::Vector2& initialXY);
            Core::Transform mouseMove( const Engine::Camera& cam, const Core::Vector2& nextXY);
        private:
            Core::Vector2 m_initialPix;
            int m_selectedAxis;
        };
    }
}
#endif // RADIUMENGINE_ROTATE_GIZMO_HPP_
