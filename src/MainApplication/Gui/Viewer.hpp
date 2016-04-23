#ifndef RADIUMENGINE_VIEWER_HPP
#define RADIUMENGINE_VIEWER_HPP

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Gui/Viewer.hpp>

namespace Ra
{
    namespace Gui
    {
        // FIXME (Charly) : Which way do we want to be able to change renderers ?
        //                  Can it be done during runtime ? Must it be at startup ? ...
        //                  For now, default ForwardRenderer is used.

        /// The Viewer is the main display class. It's the central screen QWidget.
        /// Its acts as a bridge between the interface, the engine and the renderer
        /// Among its responsibilities are :
        /// * Owning the renderer and camera, and managing their lifetime.
        /// * setting up the renderer and camera by keeping it informed of interfaces changes
        //  (e.g. resize).
        /// * catching user interaction (mouse clicks) at the lowest level and forward it to
        /// the camera and the rest of the application
        /// * Expose the asynchronous rendering interface
        class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
        {
            Q_OBJECT

        public:
            /// CONSTRUCTOR
            explicit Viewer( QWidget* parent = nullptr );

            /// DESTRUCTOR
            ~Viewer();

            /// Read-only access to renderer
            const Engine::Renderer* getRenderer() const {  return m_viewer->getRenderer(); }
            const Guibase::Viewer* getViewer() const { return m_viewer; }
            Guibase::Viewer* getViewer() { return m_viewer; }

            void render(const Scalar dt) { m_dt = dt; update(); }

            void handleFileLoading( const std::string& file ) { m_viewer->handleFileLoading(file); }

            /// Emits signals corresponding to picking requests.
            void processPicking();

            void fitCameraToScene( const Core::Aabb& sceneAabb );

        signals:
            void rendererReady();
            void leftClickPicking(int id);
            void rightClickPicking(int id);

        public slots:
            /// Tell the renderer to reload all shaders.
            void reloadShaders();
            void displayTexture(const QString& tex) { m_viewer->displayTexture(tex.toStdString()); }
            void enablePostProcess(int enabled) { m_viewer->enablePostProcess(enabled); }

            void setGizmoLocal(bool toggle) { m_viewer->getGizmoManager()->setLocal(toggle); }
            void handlePickingResult(int id) { m_viewer->getGizmoManager()->handlePickingResult(id); }
            void setGizmoEditable(Engine::EditableInterface* edit) { m_viewer->getGizmoManager()->setEditable(edit); }

        private:
            /// QOpenGlWidget primitives

            /// Initialize openGL. Called on by the first "show" call to the main window.
            virtual void initializeGL() override;

            /// Resize the view port and the camera. Called by the resize event.
            virtual void resizeGL( int width, int height ) override;

            virtual void paintGL() override { m_viewer->render(m_dt); }

            /// INTERACTION

            virtual void keyPressEvent( QKeyEvent* event ) override;
            virtual void keyReleaseEvent( QKeyEvent* event ) override;

            /// We intercept the mouse events in this widget to get the coordinates of the mouse
            /// in screen space.
            virtual void mousePressEvent( QMouseEvent* event ) override;
            virtual void mouseReleaseEvent( QMouseEvent* event ) override;
            virtual void mouseMoveEvent( QMouseEvent* event ) override;
            virtual void wheelEvent( QWheelEvent* event ) override;

        private:
            Scalar m_dt;
            Guibase::Viewer* m_viewer;
        };

    } // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_VIEWER_HPP
