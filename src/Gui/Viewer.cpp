#include <Gui/Viewer.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Event/EventEnums.hpp>
#include <Core/Event/Key.hpp>

#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>

#include <Gui/Camera/TrackballCamera.hpp>

namespace Ra
{
    namespace Gui
    {
        Viewer::Viewer()
            : m_gizmoManager(new GizmoManager(this))
        {
        }

        Viewer::~Viewer()
        {
        }

        void Viewer::initialize(uint width, uint height)
        {
            m_width = width;
            m_height = height;

#if defined (OS_WINDOWS)
            glewExperimental = GL_TRUE;

            GLuint result = glewInit();
            if ( result != GLEW_OK )
            {
                std::string errorStr;
                Ra::Core::StringUtils::stringPrintf( errorStr, " GLEW init failed : %s", glewGetErrorString( result ) );
                CORE_ERROR( errorStr.c_str() );
            }
            else
            {
                LOG( logINFO ) << "GLEW     : " << glewGetString( GLEW_VERSION );
                glFlushError();
            }
#endif

            LOG( logINFO ) << "*** Radium Engine Viewer ***";
            LOG( logINFO ) << "Renderer : " << glGetString( GL_RENDERER );
            LOG( logINFO ) << "Vendor   : " << glGetString( GL_VENDOR );
            LOG( logINFO ) << "OpenGL   : " << glGetString( GL_VERSION );
            LOG( logINFO ) << "GLSL     : " << glGetString( GL_SHADING_LANGUAGE_VERSION );

            m_renderer = Core::make_shared<Engine::ForwardRenderer>(width, height);
            m_renderer->initialize();

            m_cameras.push_back(Core::make_shared<TrackballCamera>(width, height));
            m_currentCamera = m_cameras.back().get();

            auto light = Core::make_shared<Engine::DirectionalLight>();
            m_renderer->addLight(light);
            m_currentCamera->attachLight(light);
        }

        void Viewer::render(const Scalar dt)
        {
            Engine::RenderData data;
            data.dt = dt;
            data.projMatrix = m_currentCamera->getProjMatrix();
            data.viewMatrix = m_currentCamera->getViewMatrix();
            m_renderer->render(data);
        }

        void Viewer::resize(uint width, uint height)
        {
            m_width = width;
            m_height = height;

            for (auto& cam : m_cameras) cam->resizeViewport(width, height);
            m_renderer->resize(width, height);
        }

        void Viewer::processPicking(std::vector<int>& leftClickQueries, std::vector<int>& middleClickQueries, std::vector<int>& rightClickQueries)
        {
            const auto& queries = m_renderer->getPickingQueries();
            const auto& results = m_renderer->getPickingResults();

            auto count = results.size();

            CORE_ASSERT( queries.size() == count, "There should be one result per query." );

            for (uint i = 0 ; i < count; ++i)
            {
                const auto& query = queries[i];
                const auto& result = results[i];
                switch (query.m_button)
                {
                    case Core::MouseButton_Left:
                    {
                        leftClickQueries.push_back(result);
                    }
                    break;

                    case Core::MouseButton_Middle:
                    {
                        middleClickQueries.push_back(result);
                    }
                    break;

                    case Core::MouseButton_Right:
                    {
                        rightClickQueries.push_back(result);
                    }
                    break;

                    default:
                    break;
                }
            }
        }

        void Viewer::handleEvents()
        {
            using namespace Core;
            m_currentCamera->handleEvents();
            m_gizmoManager->handleEvents();

            const auto& ins = Engine::RadiumEngine::getInstance()->getInputStatus();

            for (size_t i = 0; i < MouseButton_Count; ++i)
            {
                if (ins.buttonWasPressed[i])
                {
                    Engine::Renderer::PickingQuery query = { ins.mousePosition, (MouseButton)i };
                    m_renderer->addPickingRequest(query);
                }
            }

            if (ins.keyWasPressed[Key_Z])
            {
                m_renderer->toggleWireframe();
            }
        }
    }
}
