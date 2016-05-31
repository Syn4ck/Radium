#include <QRadium/Gui/Viewer.hpp>

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

#include <QRadium/Gui/MainWindow.hpp>
#include <QRadium/MainApplication.hpp>
#include <QRadium/EventTranslator.hpp>

namespace QRadium
{
    Viewer::Viewer( QWidget* parent )
        : QOpenGLWidget( parent )
    {
        // Create default format for Qt.
        QSurfaceFormat format;
        format.setVersion( 4, 4 );
        format.setProfile( QSurfaceFormat::CoreProfile );
        format.setDepthBufferSize( 24 );
        format.setStencilBufferSize( 8 );
        format.setSamples( 16 );
        format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
        format.setSwapInterval( 0 );
        QSurfaceFormat::setDefaultFormat( format );

        // Allow Viewer to receive events
        setFocusPolicy( Qt::StrongFocus );
        setMinimumSize( QSize( 800, 600 ) );

        m_viewer = new Ra::Gui::Viewer;
    }

    Viewer::~Viewer()
    {
        delete m_viewer;
    }

    void Viewer::initializeGL()
    {
        initializeOpenGLFunctions();

        m_viewer->initialize(width(), height());

        static_cast<MainWindow*>(parent())->onRendererReady();
    }

    void Viewer::resizeGL(int width, int height)
    {
        m_viewer->resize(width, height);
    }

    void Viewer::mousePressEvent( QMouseEvent* event )
    {
        int button = (int)getCoreButton(event);
        auto& ins = Ra::Engine::RadiumEngine::getInstance()->getInputStatus();
        ins.buttonWasPressed[button] = true;
        ins.buttonIsPressed[button] = true;

        QOpenGLWidget::mousePressEvent(event);
    }

    void Viewer::mouseReleaseEvent( QMouseEvent* event )
    {
        int button = (int)getCoreButton(event);
        auto& ins = Ra::Engine::RadiumEngine::getInstance()->getInputStatus();
        ins.buttonWasReleased[button] = true;
        ins.buttonIsPressed[button] = false;

        QOpenGLWidget::mouseReleaseEvent(event);
    }

    void Viewer::wheelEvent( QWheelEvent* event )
    {
        Ra::Engine::RadiumEngine::getInstance()->getInputStatus().wheelDelta = Ra::Core::Vector2(event->x(), event->y());

        QOpenGLWidget::wheelEvent(event);
    }

    void Viewer::keyPressEvent( QKeyEvent* event )
    {
        using namespace Ra::Core;
        auto& ins = Ra::Engine::RadiumEngine::getInstance()->getInputStatus();
        int key = (int)getCoreKey(event);

        switch (key)
        {
            case Key_Control:
            {
                ins.modifiers |= Modifier_Ctrl;
            }
            break;

            case Key_Shift:
            {
                ins.modifiers |= Modifier_Shift;
            }
            break;

            case Key_Alt:
            {
                ins.modifiers |= Modifier_Alt;
            }
            break;

            default:
            {
            }
        }

        ins.keyWasPressed[key] = true;
        ins.keyIsPressed[key] = true;

        //QOpenGLWidget::keyPressEvent(event);
    }

    void Viewer::keyReleaseEvent( QKeyEvent* event )
    {
        using namespace Ra::Core;
        auto& ins = Ra::Engine::RadiumEngine::getInstance()->getInputStatus();
        int key = (int)getCoreKey(event);

        switch (key)
        {
            case Key_Control:
            {
                ins.modifiers ^= Modifier_Ctrl;
            }
            break;

            case Key_Shift:
            {
                ins.modifiers ^= Modifier_Shift;
            }
            break;

            case Key_Alt:
            {
                ins.modifiers ^= Modifier_Alt;
            }
            break;

            default:
            {
            }
        }

        ins.keyWasReleased[key] = true;
        ins.keyIsPressed[key] = false;

        QOpenGLWidget::keyPressEvent(event);
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

    void Viewer::fitCameraToScene( const Ra::Core::Aabb& aabb )
    {
        m_viewer->getCurrentCamera()->fitScene(aabb);
    }
} // namespace QRadium
