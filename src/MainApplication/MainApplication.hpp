#include <chrono>
#include <memory>
#include <vector>

#include <QApplication>

#include <Gui/Application.hpp>
#include <MainApplication/Gui/Viewer.hpp>

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
    class MainApplication : public Guibase::Application
    {
    public:
        MainApplication(int argc, char** argv);
        virtual ~MainApplication();

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
        void framesCountForStatsChanged( int count );
        void appNeedsToQuit();

    protected:
        virtual void initializeInternal() override;
        virtual void updateUi(Plugins::RadiumPluginInterface* plugin) override;

        virtual void frameStart() override;
        virtual void handleEvents() override;
        virtual void handlePicking() override;
        virtual void render() override;
        virtual void frameEnd() override;

        virtual void runStart() override {}
        virtual void runEnd() override { exit(); }

        virtual const Engine::Renderer* getRenderer() const override { return m_viewer->getRenderer(); }

    private:
        /// Create signal / slots connections
        void createConnections();
        void setupScene();
        void addBasicShaders();

        // Public variables, accessible through the mainApp singleton.
    public:
        /// Application main window and GUI root class.
        Gui::MainWindow* m_mainWindow;

    private:
        /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Gui::Viewer* m_viewer;

        int m_frameCountBeforeUpdate;
        std::vector<FrameTimerData> m_timerData;

        QString m_fileToLoad;
    };
}


