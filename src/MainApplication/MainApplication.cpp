#include <MainApplication/MainApplication.hpp>

#include <Core/CoreMacros.hpp>

#include <QTimer>
#include <QDir>
#include <QPluginLoader>
#include <QCommandLineParser>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/String/StringUtils.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Renderers/DebugRender.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/Version.hpp>

#include <Gui/PluginBase/RadiumPluginInterface.hpp>

// Const parameters : TODO : make config / command line options

namespace Ra
{
    MainApplication::MainApplication(int argc, char** argv)
        : Guibase::Application(argc, argv)
        , m_mainWindow( nullptr )
        , m_viewer( nullptr )
        , m_fileToLoad("")
    {
        LOG(logINFO) << "build: "<<Version::compiler<<" - "<<Version::compileDate<<" "<<Version::compileTime;
        LOG(logINFO) << "Qt Version: " << qVersion();

        QCommandLineParser parser;
        parser.setApplicationDescription("Radium Engine RPZ, TMTC");
        parser.addHelpOption();
        parser.addVersionOption();

        // For any reason, the third parameter must be set if you want to be able to read anything from it (and it cannot be "")
        QCommandLineOption fpsOpt(QStringList{"r", "framerate", "fps"}, "Control the application framerate, 0 to disable it (and run as fast as possible)", "60");
        QCommandLineOption pluginOpt(QStringList{"p", "plugins", "pluginsPath"}, "Set the path to the plugin dlls", "../Plugins/bin");
        QCommandLineOption fileOpt(QStringList{"f", "file", "scene"}, "Open a scene file at startup", "foo.bar");
        // NOTE(Charly): Add other options here

        parser.addOptions({fpsOpt, pluginOpt, fileOpt});
        parser.process(*this);

        if (parser.isSet(fpsOpt))      m_targetFPS   = parser.value(fpsOpt).toUInt();
        if (parser.isSet(pluginOpt))   m_pluginsPath = parser.value(pluginOpt).toStdString();
        if (parser.isSet(fileOpt))     m_fileToLoad  = parser.value(fileOpt);

        // Create main window.
        m_mainWindow = new Gui::MainWindow;
        m_mainWindow->show();
    }

    void MainApplication::initializeInternal()
    {
        addBasicShaders();

        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        createConnections();

        setupScene();
        emit starting();

        if (m_fileToLoad.size() > 0)
        {
            loadFile(m_fileToLoad);
        }
    }

    void MainApplication::createConnections()
    {
        QCoreApplication::connect(m_mainWindow, &Gui::MainWindow::closed , this, &MainApplication::appNeedsToQuit);
    }

    void MainApplication::setupScene()
    {
        using namespace Engine::DrawPrimitives;

        Engine::SystemEntity::uiCmp()->addRenderObject(
            Primitive(Engine::SystemEntity::uiCmp(), Grid(
                    Core::Vector3::Zero(), Core::Vector3::UnitX(),
                    Core::Vector3::UnitZ(), Core::Colors::Grey(0.6f))));
    }

    void MainApplication::loadFile( QString path )
    {
        std::string pathStr = path.toLocal8Bit().data();
        LOG(logINFO) << "Loading file " << pathStr << "...";
        bool res = m_engine->loadFile( pathStr );
        CORE_UNUSED( res );
        m_viewer->handleFileLoading( pathStr );

        // Compute new scene aabb
        Core::Aabb aabb;

        std::vector<std::shared_ptr<Engine::RenderObject>> ros;
        m_engine->getRenderObjectManager()->getRenderObjects( ros );

        for ( auto ro : ros )
        {
            auto mesh = ro->getMesh();
            auto pos = mesh->getGeometry().m_vertices;

            for ( auto& p : pos )
            {
                p = ro->getLocalTransform() * p;
            }

            Ra::Core::Vector3 bmin = pos.getMap().rowwise().minCoeff().head<3>();
            Ra::Core::Vector3 bmax = pos.getMap().rowwise().maxCoeff().head<3>();

            aabb.extend( bmin );
            aabb.extend( bmax );
        }

        m_viewer->fitCameraToScene( aabb );

        emit loadComplete();
    }

    void MainApplication::framesCountForStatsChanged( int count )
    {
        m_frameCountBeforeUpdate = count;
    }

    void MainApplication::addBasicShaders()
    {
        using namespace Ra::Engine;

        ShaderConfiguration bpConfig("BlinnPhong");
        bpConfig.addShader(ShaderType_VERTEX, "../Shaders/BlinnPhong.vert.glsl");
        bpConfig.addShader(ShaderType_FRAGMENT, "../Shaders/BlinnPhong.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(bpConfig);

        ShaderConfiguration bpwConfig("BlinnPhongWireframe");
        bpwConfig.addShader(ShaderType_VERTEX, "../Shaders/BlinnPhongWireframe.vert.glsl");
        bpwConfig.addShader(ShaderType_FRAGMENT, "../Shaders/BlinnPhongWireframe.frag.glsl");
        bpwConfig.addShader(ShaderType_GEOMETRY, "../Shaders/BlinnPhongWireframe.geom.glsl");
        ShaderConfigurationFactory::addConfiguration(bpwConfig);

        ShaderConfiguration pConfig("Plain");
        pConfig.addShader(ShaderType_VERTEX, "../Shaders/Plain.vert.glsl");
        pConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Plain.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(pConfig);

        ShaderConfiguration lgConfig("LinesGeom");
        lgConfig.addShader(ShaderType_VERTEX, "../Shaders/Lines.vert.glsl");
        lgConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Lines.frag.glsl");
        lgConfig.addShader(ShaderType_GEOMETRY, "../Shaders/Lines.geom.glsl");
        ShaderConfigurationFactory::addConfiguration(lgConfig);

        ShaderConfiguration lConfig("Lines");
        lConfig.addShader(ShaderType_VERTEX, "../Shaders/Lines.vert.glsl");
        lConfig.addShader(ShaderType_FRAGMENT, "../Shaders/Lines.frag.glsl");
        ShaderConfigurationFactory::addConfiguration(lConfig);
    }

    void MainApplication::updateUi(Plugins::RadiumPluginInterface *plugin)
    {
        m_mainWindow->updateUi(plugin);
    }

    void MainApplication::frameStart()
    {
    }

    void MainApplication::handleEvents()
    {
        processEvents();
    }

    void MainApplication::handlePicking()
    {
        m_viewer->processPicking();
    }

    void MainApplication::render()
    {
        m_viewer->render(m_dt);
    }

    void MainApplication::frameEnd()
    {
        m_timerData.push_back( m_frameData );

        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
        {
            emit( updateFrameStats( m_timerData ) );
            m_timerData.clear();
        }

        m_mainWindow->onFrameComplete();
    }

    void MainApplication::appNeedsToQuit()
    {
        LOG(logDEBUG) << "About to quit.";
        m_running = false;
    }

    MainApplication::~MainApplication()
    {
        LOG( logINFO ) << "About to quit... Cleaning RadiumEngine memory";
        emit stopping();
        m_mainWindow->cleanup();
    }


}
