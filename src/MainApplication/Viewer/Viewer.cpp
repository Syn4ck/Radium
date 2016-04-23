#include <MainApplication/Viewer/Viewer.hpp>

#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>

#include <Core/String/StringUtils.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/MakeShared.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>
#include <Engine/Renderer/Renderers/DeferredRenderer.hpp>

#include <MainApplication/Viewer/TrackballCamera.hpp>
#include <MainApplication/Viewer/Gizmo/GizmoManager.hpp>
#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Utils/Keyboard.hpp>

namespace Ra
{
    Gui::Viewer::Viewer(QWidget* parent)
        : QOpenGLWidget(parent)
        , m_gizmoManager(new GizmoManager(this))
    {
        // Allow Viewer to receive events
        setFocusPolicy(Qt::StrongFocus);
        setMinimumSize(QSize(800, 600));

        m_camera.reset(new Gui::TrackballCamera(width(), height()));
    }

    Gui::Viewer::~Viewer()
    {
    }

    void Gui::Viewer::initializeGL()
    {
        initializeOpenGLFunctions();

        LOG(logINFO) << "*** Radium Engine Viewer ***";
        LOG(logINFO) << "Renderer : " << glGetString(GL_RENDERER);
        LOG(logINFO) << "Vendor   : " << glGetString(GL_VENDOR);
        LOG(logINFO) << "OpenGL   : " << glGetString(GL_VERSION);
        LOG(logINFO) << "GLSL     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

#if defined(OS_WINDOWS)
        glewExperimental = GL_TRUE;

        GLuint result = glewInit();
        if (result != GLEW_OK)
        {
            std::string errorStr;
            Ra::Core::StringUtils::stringPrintf(errorStr, " GLEW init failed : %s", glewGetErrorString(result));
            CORE_ERROR(errorStr.c_str());
        }
        else
        {
            LOG(logINFO) << "GLEW     : " << glewGetString(GLEW_VERSION);
            glFlushError();
        }

#endif

        LOG(logDEBUG) << "Rendering on main thread";

        // FIXME(Charly): Renderer type should not be changed here
        m_renderers.resize(2);
        m_renderers[1].reset(new Engine::ForwardRenderer(width(), height()));
        m_renderers[0].reset(new Engine::DeferredRenderer(width(), height()));

        for (auto& renderer : m_renderers)
        {
            renderer->initialize();
        }

        m_currentRenderer = m_renderers[0].get();

        auto light = Ra::Core::make_shared<Ra::Engine::Light>(Ra::Engine::Light::LightType_Directional);

        for (auto& renderer : m_renderers)
        {
            renderer->addLight(light);
        }

        m_camera->attachLight(light);

        emit rendererReady();
    }

    void Gui::Viewer::initRenderer()
    {
    }

    void Gui::Viewer::resizeGL(int width, int height)
    {
        // Renderer should have been locked by previous events.
        m_camera->resizeViewport(width, height);
        m_currentRenderer->resize(width, height);
    }

    void Gui::Viewer::mousePressEvent(QMouseEvent* event)
    {
        switch (event->button())
        {
            case Qt::LeftButton:
            {
                if (isKeyPressed(Key_Space))
                {
                    LOG(logINFO) << "Raycast query launched";
                    Core::Ray r = m_camera->getCamera()->getRayFromScreen(Core::Vector2(event->x(), event->y()));
                    RA_DISPLAY_POINT(r.m_origin, Core::Colors::Cyan(), 0.1f);
                    RA_DISPLAY_RAY(r, Core::Colors::Yellow());
                    auto ents = mainApp->getEngine()->getEntityManager()->getEntities();
                    for (auto e : ents)
                    {
                        e->rayCastQuery(r);
                    }
                }
                else
                {
                    Engine::Renderer::PickingQuery query = { Core::Vector2(event->x(), height() - event->y()), Core::MouseButton::RA_MOUSE_LEFT_BUTTON };
                    m_currentRenderer->addPickingRequest(query);
                    m_gizmoManager->handleMousePressEvent(event);
                }
            }
            break;

            case Qt::MiddleButton:
            {
                m_camera->handleMousePressEvent(event);
            }
            break;

            case Qt::RightButton:
            {
                // Check picking
                Engine::Renderer::PickingQuery query = { Core::Vector2(event->x(), height() - event->y()), Core::MouseButton::RA_MOUSE_RIGHT_BUTTON };
                m_currentRenderer->addPickingRequest(query);
            }
            break;

            default:
            {
            }
            break;
        }
    }

    void Gui::Viewer::mouseReleaseEvent(QMouseEvent* event)
    {
        m_camera->handleMouseReleaseEvent(event);
        m_gizmoManager->handleMouseReleaseEvent(event);
    }

    void Gui::Viewer::mouseMoveEvent(QMouseEvent* event)
    {
        m_camera->handleMouseMoveEvent(event);
        m_gizmoManager->handleMouseMoveEvent(event);
    }

    void Gui::Viewer::wheelEvent(QWheelEvent* event)
    {
        m_camera->handleWheelEvent(event);
        QOpenGLWidget::wheelEvent(event);
        mainApp->m_mainWindow->viewerWheelEvent(event);
    }

    void Gui::Viewer::keyPressEvent(QKeyEvent* event)
    {
        m_camera->handleKeyPressEvent(event);

        QOpenGLWidget::keyPressEvent(event);
    }

    void Gui::Viewer::keyReleaseEvent(QKeyEvent* event)
    {
        m_camera->handleKeyReleaseEvent(event);

        if (event->key() == Qt::Key_Z && !event->isAutoRepeat())
        {
            m_currentRenderer->toggleWireframe();
        }

        QOpenGLWidget::keyReleaseEvent(event);
    }

    void Gui::Viewer::reloadShaders()
    {
        makeCurrent();
        m_currentRenderer->reloadShaders();
        doneCurrent();
    }

    void Gui::Viewer::displayTexture(const QString& tex)
    {
        m_currentRenderer->displayTexture(tex.toStdString());
    }

    void Gui::Viewer::changeRenderer(int index)
    {
        // NOTE(Charly): This is probably buggy since it has not been tested.
        LOG(logWARNING) << "Changing renderers might be buggy since it has not been tested.";
        m_currentRenderer = m_renderers[index].get();
        m_currentRenderer->initialize();
        m_currentRenderer->resize(width(), height());

        updateTextureNames();
    }

    // Asynchronous rendering implementation

    void Gui::Viewer::startRendering(Scalar dt)
    {
        makeCurrent();

        // Move camera if needed. Disabled for now as it takes too long (see issue #69)
        // m_camera->update( dt );

        Engine::RenderData data;
        data.dt         = dt;
        data.projMatrix = m_camera->getProjMatrix();
        data.viewMatrix = m_camera->getViewMatrix();
        m_currentRenderer->render(data);
        glFinish();

        doneCurrent();
    }

    void Gui::Viewer::waitForRendering()
    {
    }

    void Gui::Viewer::handleFileLoading(const std::string& file)
    {
        for (auto& renderer : m_renderers)
        {
            renderer->handleFileLoading(file);
        }
    }

    void Gui::Viewer::processPicking()
    {
        CORE_ASSERT(m_currentRenderer->getPickingQueries().size() == m_currentRenderer->getPickingResults().size(), "There should be one result per query.");

        for (uint i = 0; i < m_currentRenderer->getPickingQueries().size(); ++i)
        {
            const Engine::Renderer::PickingQuery& query = m_currentRenderer->getPickingQueries()[i];
            if (query.m_button == Core::MouseButton::RA_MOUSE_LEFT_BUTTON)
            {
                emit leftClickPicking(m_currentRenderer->getPickingResults()[i]);
            }
            else if (query.m_button == Core::MouseButton::RA_MOUSE_RIGHT_BUTTON)
            {
                emit rightClickPicking(m_currentRenderer->getPickingResults()[i]);
            }
        }
    }

    void Gui::Viewer::fitCameraToScene(const Core::Aabb& aabb)
    {
        // FIXME(Charly): Does not work, the camera needs to be fixed
        m_camera->fitScene(aabb);
    }

    std::vector<std::string> Gui::Viewer::getRenderersName() const
    {
        std::vector<std::string> ret;

        for (const auto& r : m_renderers)
        {
            ret.push_back(r->getRendererName());
        }

        return ret;
    }

    void Gui::Viewer::enablePostProcess(int enabled)
    {
        m_currentRenderer->enablePostProcess(enabled);
    }

    void Gui::Viewer::updateTextureNames()
    {
        auto win = mainApp->m_mainWindow.get();

        QSignalBlocker blockTextures(win->m_displayedTextureCombo);

        win->m_displayedTextureCombo->clear();

        auto textures = getRenderer()->getAvailableTextures(getRenderer()->getRendererName());
        for (const auto& tex : textures)
        {
            win->m_displayedTextureCombo->addItem(tex.c_str());
        }
    }

} // namespace Ra
