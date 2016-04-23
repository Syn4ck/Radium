#include <MainApplication/Gui/Viewer.hpp>

#include <iostream>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include <Core/String/StringUtils.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/MainApplication.hpp>
#include <MainApplication/EventTranslator.hpp>

namespace Ra
{
    namespace Gui
    {
        Viewer::Viewer( QWidget* parent )
            : QOpenGLWidget( parent )
        {
            // Allow Viewer to receive events
            setFocusPolicy( Qt::StrongFocus );
            setMinimumSize( QSize( 800, 600 ) );

            m_viewer = new Guibase::Viewer;
        }

        Viewer::~Viewer()
        {
            delete m_viewer;
        }

        void Viewer::initializeGL()
        {
            initializeOpenGLFunctions();

            m_viewer->initialize(width(), height());

            emit rendererReady();
        }

        void Viewer::resizeGL(int width, int height)
        {
            m_viewer->resize(width, height);
        }

        void Viewer::mousePressEvent( QMouseEvent* event )
        {
            Core::MouseEvent e;
            qtToCore(event, &e);
            if (!m_viewer->mouseEvent(&e))
            {
                QOpenGLWidget::mousePressEvent(event);
            }
        }

        void Viewer::mouseReleaseEvent( QMouseEvent* event )
        {
            Core::MouseEvent e;
            qtToCore(event, &e);
            if (!m_viewer->mouseEvent(&e))
            {
                QOpenGLWidget::mouseReleaseEvent(event);
            }
        }

        void Viewer::mouseMoveEvent( QMouseEvent* event )
        {
            Core::MouseEvent e;
            qtToCore(event, &e);
            if (!m_viewer->mouseEvent(&e))
            {
                QOpenGLWidget::mouseMoveEvent(event);
            }
        }

        void Viewer::wheelEvent( QWheelEvent* event )
        {
            Core::MouseEvent e;
            qtToCore(event, &e);
            if (!m_viewer->mouseEvent(&e))
            {
                QOpenGLWidget::wheelEvent(event);
            }
        }

        void Viewer::keyPressEvent( QKeyEvent* event )
        {
            Core::KeyEvent e;
            qtToCore(event, &e);
            if (!m_viewer->keyEvent(&e))
            {
                QOpenGLWidget::keyPressEvent(event);
            }
        }

        void Viewer::keyReleaseEvent( QKeyEvent* event )
        {
            Core::KeyEvent e;
            qtToCore(event, &e);
            if (!m_viewer->keyEvent(&e))
            {
                QOpenGLWidget::keyReleaseEvent(event);
            }
        }

        void Viewer::reloadShaders()
        {
            makeCurrent();
            m_viewer->reloadShaders();
            doneCurrent();
        }

        void Viewer::processPicking()
        {
            std::vector<int> leftClickQueries;
            std::vector<int> rightClickQueries;
            std::vector<int> middleClickQueries;

            m_viewer->processPicking(leftClickQueries, middleClickQueries, rightClickQueries);

            for (const auto& q : leftClickQueries)
            {
                emit leftClickPicking(q);
            }

            for (const auto& q : middleClickQueries)
            {
                CORE_UNUSED(q);
            }

            for (const auto& q : rightClickQueries)
            {
                emit rightClickPicking(q);
            }
        }

        void Viewer::fitCameraToScene( const Core::Aabb& aabb )
        {
            m_viewer->getCurrentCamera()->fitScene(aabb);
        }
    }

} // namespace Ra
