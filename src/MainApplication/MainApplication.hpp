#include <chrono>
#include <memory>
#include <vector>

#include <QApplication>

#include <Core/Time/Timer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>
#include <GuiBase/Viewer/Viewer.hpp>

class QTimer;
namespace Ra
{
    namespace Core
    {
        class TaskQueue;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace Ra
{
    namespace Gui
    {
        class Viewer;
        class MainWindow;
    }
}

/// Allow singleton-like access to the main app à la qApp.
#if defined(mainApp)
#undef mainApp
#endif
#define mainApp (static_cast<Ra::MainApplication*>(qApp))

namespace Ra
{
    /// This class contains the main application logic. It owns the engine and the GUI.
    class MainApplication : public QApplication
    {
        Q_OBJECT

    public:
        MainApplication( int argc, char** argv );
        ~MainApplication();

        /// Advance the engine for one frame.
        void radiumFrame();

        bool isRunning() const { return !m_isAboutToQuit; }

        const Engine::RadiumEngine* getEngine () const { return m_engine.get();}

    signals:
        /// Fired when the engine has just started, before the frame timer is set.
        void starting();

        /// Fired when the engine is about to stop.
        void stopping();

        /// Fired when the scene has changed.
        void sceneChanged( const Core::Aabb& );

        void updateFrameStats( const std::vector<FrameTimerData>& );

        void loadComplete();

    public slots:

        void loadFile( QString path );
        void framesCountForStatsChanged( uint count );
        void appNeedsToQuit();
        void setRealFrameRate( bool on);

    private:
        /// Create signal / slots connections
        void createConnections();

        /// This function is defined in a separate file called "Systems.cpp"
        /// The content of this file is auto-generated by cmake to register
        /// all plugins in the Plugins/ folder.
        bool loadPlugins( const std::string& pluginsPath );

        void setupScene();
        void addBasicShaders();


        // Public variables, accessible through the mainApp singleton.
    public:
        /// Application main window and GUI root class.
        std::unique_ptr<Gui::MainWindow> m_mainWindow;

        /// Instance of the radium engine.
        std::unique_ptr<Engine::RadiumEngine> m_engine;

<<<<<<< 84ab4a814076df7c6fddc158a10d333b5e9a1457:src/MainApplication/MainApplication.hpp
        /// Task queue for processing tasks.
        std::unique_ptr<Core::TaskQueue> m_taskQueue;

        /// Number of frames per second to generate.
=======
>>>>>>> Moving some rendering to Viewer::paintGL:MainApplication/MainApplication.hpp
        uint m_targetFPS;

    private:
        /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Gui::Viewer* m_viewer;

        /// Timer to wake us up at every frame start.
        QTimer* m_frameTimer;

        /// Time since the last frame start.
        Core::Timer::TimePoint m_lastFrameStart;

        uint m_frameCounter;
        uint m_frameCountBeforeUpdate;
        uint m_numFrames;
        std::vector<FrameTimerData> m_timerData;

        /// If true, use the wall clock to advance the engine. If false, use a fixed time step.
        bool m_realFrameRate;
        bool m_isAboutToQuit;
    };
}


