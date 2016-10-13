#include <MainApplication.hpp>

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

#include "Gui/MainWindow.hpp"

#include <PluginBase/RadiumPluginInterface.hpp>

// Const parameters : TODO : make config / command line options


namespace Ra
{
    MainApplication::MainApplication( int argc, char** argv, QString applicationName, QString organizationName)
        : QApplication( argc, argv )
        , m_mainWindow( nullptr )
        , m_engine( nullptr )
        , m_viewer( nullptr )
        , m_frameTimer( new QTimer( this ) )
        , m_frameCounter( 0 )
        , m_numFrames( 0 )
        , m_isAboutToQuit( false )
        //, m_timerData(TIMER_AVERAGE)
    {
        // Set application and organization names in order to ensure uniform
        // QSettings configurations.
        // \see http://doc.qt.io/qt-5/qsettings.html#QSettings-4
        QCoreApplication::setOrganizationName(organizationName);
        QCoreApplication::setApplicationName(applicationName);

        m_targetFPS = 60; // Default
        std::string pluginsPath = "../Plugins/bin";

        QCommandLineParser parser;
        parser.setApplicationDescription("Radium Engine RPZ, TMTC");
        parser.addHelpOption();
        parser.addVersionOption();

        // For any reason, the third parameter must be set if you want to be able to read anything from it (and it cannot be "")
        QCommandLineOption fpsOpt(QStringList{"r", "framerate", "fps"}, "Control the application framerate, 0 to disable it (and run as fast as possible)", "60");
        QCommandLineOption pluginOpt(QStringList{"p", "plugins", "pluginsPath"}, "Set the path to the plugin dlls", "../Plugins/bin");
        QCommandLineOption fileOpt(QStringList{"f", "file", "scene"}, "Open a scene file at startup", "foo.bar");
        QCommandLineOption numFramesOpt(QStringList{"n", "numframes"}, "Run for a fixed number of frames", "0");
        // NOTE(Charly): Add other options here

        parser.addOptions({fpsOpt, pluginOpt, fileOpt, numFramesOpt });
        parser.process(*this);

        if (parser.isSet(fpsOpt))       m_targetFPS = parser.value(fpsOpt).toUInt();
        if (parser.isSet(pluginOpt))    pluginsPath = parser.value(pluginOpt).toStdString();
        if (parser.isSet(numFramesOpt)) m_numFrames = parser.value(numFramesOpt).toUInt();

        // Boilerplate print.
        LOG( logINFO ) << "*** Radium Engine Main App  ***";
        std::stringstream config;
#if defined (CORE_DEBUG)
        config << "(Debug Build) -- ";
#else
        config << "(Release Build) -- ";
#endif

#if defined (ARCH_X86)
        config << " 32 bits x86";
#elif defined (ARCH_X64)
        config << " 64 bits x64";
#endif
        LOG( logINFO ) << config.str();

        config.str( std::string() );
        config << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
        config << "double precision";
#else
        config << "single precision" ;
#endif

        LOG( logINFO ) << config.str();

        config.str( std::string() );
        //config<<"build: "<<Version::compiler<<" - "<<Version::compileDate<<" "<<Version::compileTime;


        LOG( logINFO ) << config.str();

        LOG(logINFO) << "Qt Version: " << qVersion();

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

        // Create engine
        m_engine.reset(Engine::RadiumEngine::createInstance());
        m_engine->initialize();

        // Create main window.
        m_mainWindow.reset( new Gui::MainWindow );
        m_mainWindow->show();

        addBasicShaders();

        // Allow all events to be processed (thus the viewer should have
        // initialized the OpenGL context..)
        processEvents();

        // Load plugins
        if ( !loadPlugins( pluginsPath ) )
        {
            LOG( logERROR ) << "An error occurred while trying to load plugins.";
        }

        m_viewer = m_mainWindow->getViewer();
        CORE_ASSERT( m_viewer != nullptr, "GUI was not initialized" );
        CORE_ASSERT( m_viewer->context()->isValid(), "OpenGL was not initialized" );

        // Pass the engine to the renderer to complete the initialization process.
        m_viewer->initRenderer();

        createConnections();

        setupScene();
        emit starting();

        // A file has been required, load it.
        if (parser.isSet(fileOpt))
        {
            loadFile(parser.value(fileOpt));
        }

        // Update the window every 1/60 sec.
        connect( m_frameTimer, &QTimer::timeout, m_mainWindow.get(), &Ra::Gui::MainWindow::update );
        m_frameTimer->start((m_targetFPS > 0) ? 1000/m_targetFPS : 0);

        m_lastFrameStart = Core::Timer::Clock::now();
    }

    void MainApplication::createConnections()
    {
        connect( m_mainWindow.get(), &Gui::MainWindow::closed , this, &MainApplication::appNeedsToQuit );
    }

    void MainApplication::setupScene()
    {
        using namespace Engine::DrawPrimitives;

        Engine::SystemEntity::uiCmp()->addRenderObject(
            Primitive(Engine::SystemEntity::uiCmp(), Grid(
                    Core::Vector3::Zero(), Core::Vector3::UnitX(),
                    Core::Vector3::UnitZ(), Core::Colors::Grey(0.6f))));

        Engine::SystemEntity::uiCmp()->addRenderObject(
                    Primitive(Engine::SystemEntity::uiCmp(), Frame(Ra::Core::Transform::Identity(), 0.05f)));

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

    void MainApplication::framesCountForStatsChanged( uint count )
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

    void MainApplication::radiumFrame()
    {
        FrameTimerData timerData;
        timerData.frameStart = Core::Timer::Clock::now();

        // ----------
        // 0. Compute time since last frame.
        const Scalar dt = m_realFrameRate ?
                    Core::Timer::getIntervalSeconds( m_lastFrameStart, timerData.frameStart ) :
                    1.f / Scalar(m_targetFPS);
        m_lastFrameStart = timerData.frameStart;

        timerData.eventsStart = Core::Timer::Clock::now();
        processEvents();
        timerData.eventsEnd = Core::Timer::Clock::now();

        // ----------
        // 1. Gather user input and dispatch it.

        // Get picking results from last frame and forward it to the selection.
        m_viewer->processPicking();


        // ----------
        // 2. Kickoff rendering
        m_viewer->m_dt = dt;
//        m_viewer->update();
        m_viewer->startRendering( dt );

/*
        timerData.tasksStart = Core::Timer::Clock::now();
        // ----------
        // 3. Run the engine task queue.
        m_engine->getTasks( m_taskQueue.get(), dt );

        // Run one frame of tasks
        m_taskQueue->startTasks();
        m_taskQueue->waitForTasks();
        timerData.taskData = m_taskQueue->getTimerData();
        m_taskQueue->flushTaskQueue();

        timerData.tasksEnd = Core::Timer::Clock::now();
*/
        // ----------
        // 4. Wait until frame is fully rendered and display.
        m_viewer->waitForRendering();
        m_viewer->update();

        timerData.renderData = m_viewer->getRenderer()->getTimerData();

        // ----------
        // 5. Synchronize whatever needs synchronisation
        m_engine->endFrameSync();

        // ----------
        // 6. Frame end.
        timerData.frameEnd = Core::Timer::Clock::now();
        timerData.numFrame = m_frameCounter;

        m_timerData.push_back( timerData );
        ++m_frameCounter;

        if (m_numFrames > 0  &&  m_frameCounter > m_numFrames )
        {
            appNeedsToQuit();
        }

        if ( m_frameCounter % m_frameCountBeforeUpdate == 0 )
        {
            emit( updateFrameStats( m_timerData ) );
            m_timerData.clear();
        }

        m_mainWindow->onFrameComplete();
    }

    void MainApplication::appNeedsToQuit()
    {
        LOG( logDEBUG ) << "About to quit.";
        m_isAboutToQuit = true;
    }

    void MainApplication::setRealFrameRate(bool on)
    {
       m_realFrameRate = on;
    }

    MainApplication::~MainApplication()
    {
        LOG( logINFO ) << "About to quit... Cleaning RadiumEngine memory";
        emit stopping();
        m_mainWindow->cleanup();
        m_engine->cleanup();
    }

    bool MainApplication::loadPlugins( const std::string& pluginsPath )
    {
        LOG( logINFO )<<" *** Loading Plugins ***";
        QDir pluginsDir( qApp->applicationDirPath() );
        pluginsDir.cd( pluginsPath.c_str() );

        bool res = true;
        uint pluginCpt = 0;

        for (const auto& filename : pluginsDir.entryList(QDir::Files))
//        foreach (QString filename, pluginsDir.entryList( QDir::Files ) )
        {
            PluginContext context;
            context.m_engine = m_engine.get();
            context.m_selectionManager = m_mainWindow->getSelectionManager();

            std::string ext = Core::StringUtils::getFileExt( filename.toStdString() );
#if defined( OS_WINDOWS )
            std::string sysDllExt = "dll";
#elif defined( OS_LINUX )
            std::string sysDllExt = "so";
#elif defined( OS_MACOS )
            std::string sysDllExt = "dylib";
#else
            static_assert( false, "System configuration not handled" );
#endif
            if ( ext == sysDllExt )
            {
                QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( filename ) );
                // Force symbol resolution at load time.
                pluginLoader.setLoadHints( QLibrary::ResolveAllSymbolsHint );

                LOG( logINFO ) << "Found plugin " << filename.toStdString();

                QObject* plugin = pluginLoader.instance();
                Plugins::RadiumPluginInterface* loadedPlugin;

                if ( plugin )
                {
                    loadedPlugin = qobject_cast<Plugins::RadiumPluginInterface*>( plugin );
                    if ( loadedPlugin )
                    {
                        ++pluginCpt;
                        loadedPlugin->registerPlugin( context );
                        m_mainWindow->updateUi( loadedPlugin );
                    }
                    else
                    {
                        LOG( logERROR ) << "Something went wrong while trying to cast plugin"
                                        << filename.toStdString();
                        res = false;
                    }
                }
                else
                {
                    LOG( logERROR ) << "Something went wrong while trying to load plugin "
                                    << filename.toStdString() << " : "
                                    << pluginLoader.errorString().toStdString();
                    res = false;
                }
            }
        }

        if (pluginCpt == 0)
        {
            LOG(logINFO) << "No plugin found or loaded.";
        }
        else
        {
            LOG(logINFO) << "Loaded " << pluginCpt << " plugins.";
        }

        return res;
    }
}
