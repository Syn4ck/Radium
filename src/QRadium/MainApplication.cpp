#include <QRadium/MainApplication.hpp>

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

#include <QRadium/Gui/MainWindow.hpp>
#include <QRadium/Version.hpp>

#include <Gui/PluginBase/RadiumPluginInterface.hpp>

// Const parameters : TODO : make config / command line option
namespace QRadium
{
    QRadiumApplication::QRadiumApplication(int argc, char **argv)
        : Ra::Gui::Application(argc, argv)
        , m_frameCountBeforeUpdate(60)
    {
        m_mainApplication = new MainApplication(m_argc, m_argv);
        m_mainWindow = m_mainApplication->m_mainWindow;
    }

    QRadiumApplication::~QRadiumApplication()
    {
        LOG( logINFO ) << "About to quit... Cleaning RadiumEngine memory";
        delete m_mainApplication;
    }

    void QRadiumApplication::initializeInternal()
    {
        addBasicShaders();
        setupScene();

        m_mainApplication->initializeInternal();
        m_viewer = m_mainApplication->m_viewer;

        if (m_fileToLoad.size() > 0)
        {
            loadFile(m_fileToLoad);
        }

        m_mainWindow->onEntitiesUpdated();
    }

    void QRadiumApplication::setupScene()
    {
        using namespace Ra::Engine::DrawPrimitives;

        Ra::Engine::SystemEntity::uiCmp()->addRenderObject(
                    Primitive(Ra::Engine::SystemEntity::uiCmp(), Grid(
                                  Ra::Core::Vector3::Zero(),  Ra::Core::Vector3::UnitX(),
                                  Ra::Core::Vector3::UnitZ(), Ra::Core::Colors::Grey(0.6f))));
    }

    void QRadiumApplication::loadFile(const std::string& pathStr)
    {
        LOG(logINFO) << "Loading file " << pathStr << "...";
        bool res = m_engine->loadFile( pathStr );
        CORE_UNUSED( res );
        m_viewer->handleFileLoading( pathStr );

        // Compute new scene aabb
        Ra::Core::Aabb aabb;

        std::vector<std::shared_ptr<Ra::Engine::RenderObject>> ros;
        m_engine->getRenderObjectManager()->getRenderObjects( ros );

        for ( auto ro : ros )
        {
            auto mesh = ro->getMesh();
            if (mesh )
            {
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
        }

        m_viewer->fitCameraToScene(aabb);
    }

    void QRadiumApplication::addBasicShaders()
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

    void QRadiumApplication::updateUi(Ra::Plugins::RadiumPluginInterface *plugin)
    {
        m_mainWindow->updateUi(plugin);
    }

    void QRadiumApplication::frameStart()
    {
    }

    void QRadiumApplication::handleEvents()
    {
        Ra::Engine::RadiumEngine::getInstance()->getInputStatus().resetFrameEvents();
        m_mainApplication->processEvents();
        QPoint pos = m_viewer->mapFromGlobal(QCursor::pos());
        Ra::Engine::RadiumEngine::getInstance()->getInputStatus().mousePosition = { pos.x(), pos.y() };

        QPoint p1 = QCursor::pos();
        QPoint p2 = m_viewer->mapFromGlobal(p1);

        //LOG(logINFO) << "Global : " << p1.x() << " " << p1.y() << " - Local : " << p2.x() << " " << p2.y();

        m_viewer->getViewer()->handleEvents();
    }

    void QRadiumApplication::handlePicking()
    {
        m_viewer->processPicking();
    }

    void QRadiumApplication::render()
    {
        m_viewer->render(m_dt);
    }

    void QRadiumApplication::frameEnd()
    {
        m_timerData.push_back( m_frameData );

//        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
//        {
//            m_mainWindow->onUpdateFramestats(m_timerData);
//            m_timerData.clear();
//        }

        m_mainWindow->onFrameComplete();
    }

    MainApplication::MainApplication(int& argc, char** argv)
        : QApplication(argc, argv)
        , m_mainWindow(nullptr)
        , m_viewer(nullptr)
        , m_fileToLoad("")
    {
        LOG(logINFO) << "build: "<<Ra::Version::compiler<<" - "<<Ra::Version::compileDate<<" "<<Ra::Version::compileTime;
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

        if (parser.isSet(fpsOpt))      mainApp->m_targetFPS   = parser.value(fpsOpt).toUInt();
        if (parser.isSet(pluginOpt))   mainApp->m_pluginsPath = parser.value(pluginOpt).toStdString();
        if (parser.isSet(fileOpt))     m_fileToLoad           = parser.value(fileOpt);

        // Create main window.
        m_mainWindow = new MainWindow;
        m_mainWindow->show();
    }

    MainApplication::~MainApplication()
    {
        m_mainWindow->cleanup();
    }

    void MainApplication::appNeedsToQuit()
    {
        LOG(logDEBUG) << "About to quit.";
        mainApp->m_running = false;
    }

    void MainApplication::initializeInternal()
    {
        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        createConnections();
        m_mainWindow->initialize();
    }

    void MainApplication::createConnections()
    {
        QCoreApplication::connect(m_mainWindow, &MainWindow::closed, this, &MainApplication::appNeedsToQuit);
    }

    RA_SINGLETON_IMPLEMENTATION(QRadiumApplication);
} // namespace QRadium
