#include <Gui/Application.hpp>

// FIXME(charly): All of this should go away
#include <QDir>
#include <QPluginLoader>

#include <Core/CoreMacros.hpp>

#include <Gui/PluginBase/RadiumPluginInterface.hpp>

namespace Ra
{
    namespace Guibase
    {
        Application::Application(int argc, char** argv)
            : m_engine(nullptr)
            , m_taskQueue(nullptr)
            , m_targetFPS(60)
            , m_targetFrameTime(1.0 / 60.0)
            , m_argc(argc)
            , m_argv(argv)
            , m_pluginsPath("../Plugins/bin")
        {
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

            // Create engine
            m_engine.reset(Engine::RadiumEngine::createInstance());
            m_engine->initialize();

            // Create task queue with N-1 threads (we keep one for rendering).
            m_taskQueue.reset(new Core::TaskQueue(std::thread::hardware_concurrency() - 1));
        }

        Application::~Application()
        {
            m_engine->cleanup();
        }

        void Application::run()
        {
            if (!loadPlugins(m_pluginsPath)) { LOG(logERROR) << "An error occured while trying to load plugins."; }
            initializeInternal();

            m_lastFrameStart = Core::Timer::Clock::now();

            m_running = true;
            m_frameCounter = 0;

            Ra::Core::Timer::TimePoint t0, t1;

            runStart();

            while (m_running)
            {
                t0 = Ra::Core::Timer::Clock::now();
                frame();

                Scalar remaining = m_targetFrameTime;
                while (remaining > 0.0)
                {
                    // Sleep
                    t1 = Ra::Core::Timer::Clock::now();
                    remaining -= Ra::Core::Timer::getIntervalSeconds(t0, t1);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    t0 = t1;
                }
            }

            runEnd();
        }

        void Application::frame()
        {
            // ----------
            // Frame start.
            frameStart();

            m_frameData.frameStart = Core::Timer::Clock::now();

            // ----------
            // 0. Compute time since last frame.
            m_dt = Core::Timer::getIntervalSeconds(m_lastFrameStart, m_frameData.frameStart);
            m_lastFrameStart = m_frameData.frameStart;

            // ----------
            // 1. Gather user input
            m_frameData.eventsStart = Core::Timer::Clock::now();
            handleEvents();
            m_frameData.eventsEnd = Core::Timer::Clock::now();

            // ----------
            // 2. Process picking
            m_frameData.pickingStart = Core::Timer::Clock::now();
            handlePicking();
            m_frameData.pickingEnd = Core::Timer::Clock::now();

            // ----------
            // 3. Run the engine task queue.
            m_frameData.tasksStart = Core::Timer::Clock::now();

            m_engine->getTasks(m_taskQueue.get(), m_dt);

            // Run one frame of tasks
            m_taskQueue->startTasks();
            m_taskQueue->waitForTasks();
            m_frameData.taskData = m_taskQueue->getTimerData();
            m_taskQueue->flushTaskQueue();

            m_frameData.tasksEnd = Core::Timer::Clock::now();

            // ----------
            // 4. Plan rendering
            render();
            m_frameData.renderData = getRenderer()->getTimerData();

            // ----------
            // 5. Synchronize whatever needs synchronisation
            m_frameData.endSyncStart = Core::Timer::Clock::now();
            m_engine->endFrameSync();
            m_frameData.endSyncEnd = Core::Timer::Clock::now();

            // ----------
            // Frame end.
            m_frameData.frameEnd = Core::Timer::Clock::now();
            m_frameData.numFrame = m_frameCounter++;

            frameEnd();
        }

        bool Application::loadPlugins(const std::string& pluginsPath)
        {
            LOG(logINFO) << " *** Loading Plugins ***";
            QDir pluginsDir(".");
            pluginsDir.cd(pluginsPath.c_str());

            bool res = true;
            uint pluginCpt = 0;

            for (const auto& filename : pluginsDir.entryList(QDir::Files))
                //        foreach (QString filename, pluginsDir.entryList( QDir::Files ) )
            {
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
                        if (loadedPlugin)
                        {
                            ++pluginCpt;
                            loadedPlugin->registerPlugin(m_engine.get());
                            updateUi(loadedPlugin);
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
}
