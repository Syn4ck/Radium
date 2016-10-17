#if 0

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

#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

// Const parameters : TODO : make config / command line options

namespace Ra
{
    MainApplication::MainApplication( int argc, char** argv )
        : QApplication( argc, argv )
        , m_mainWindow( nullptr )
        , m_engine( nullptr )
        , m_taskQueue( nullptr )
        , m_viewer( nullptr )
        , m_frameCounter( 0 )
        , m_numFrames( 0 )
        , m_isAboutToQuit( false )
        //, m_timerData(TIMER_AVERAGE)
    {
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

        // Create task queue with N-1 threads (we keep one for rendering).
        m_taskQueue.reset( new Core::TaskQueue( std::thread::hardware_concurrency() - 1 ) );

        m_frameTimer = new QTimer(this);
        m_frameTimer->setTimerType(Qt::PreciseTimer);
        createConnections();

        setupScene();
        emit starting();

        // A file has been required, load it.
        if (parser.isSet(fileOpt))
        {
            loadFile(parser.value(fileOpt));
        }

        int msec = 0;
        if (m_targetFPS > 0)
        {
            double sec = 1.0 / (double)m_targetFPS;
            msec = (int)(sec * 1000);
        }
        m_frameTimer->start(msec);

        m_lastFrameStart = Core::Timer::Clock::now();
    }

    void MainApplication::createConnections()
    {
        connect( m_mainWindow.get(), &Gui::MainWindow::closed , this, &MainApplication::appNeedsToQuit );
        connect(m_frameTimer, &QTimer::timeout, this, &MainApplication::radiumFrame);
    }

    void MainApplication::setupScene()
    {
    }

    void MainApplication::framesCountForStatsChanged( uint count )
    {
        m_frameCountBeforeUpdate = count;
    }

    void MainApplication::addBasicShaders()
    {
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
        //processEvents();
        timerData.eventsEnd = Core::Timer::Clock::now();

        // ----------
        // 1. Gather user input and dispatch it.

        // Get picking results from last frame and forward it to the selection.
        m_viewer->processPicking();

        // ----------
        // 2. Kickoff rendering
        //m_viewer->startRendering( dt );

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

        // ----------
        // 4. Wait until frame is fully rendered and display.
        //m_viewer->waitForRendering();
        //m_mainWindow->update();
        //m_viewer->updateGL();
        //update();

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
}

#endif 
