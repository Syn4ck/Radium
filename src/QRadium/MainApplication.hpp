#include <chrono>
#include <memory>
#include <vector>

#include <QApplication>

#include <Core/Utils/Singleton.hpp>

#include <Gui/Application.hpp>
#include <QRadium/Gui/Viewer.hpp>

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
    }
}

namespace QRadium
{
    class Viewer;
    class MainWindow;

    class MainApplication : public QApplication
    {
        friend class QRadiumApplication;

        Q_OBJECT

    public:
        MainApplication(int& argc, char** argv);
        virtual ~MainApplication();

        void initializeInternal();

    private slots:
        void appNeedsToQuit();

    private:
        /// Create signal / slots connections
        void createConnections();

        // Public variables, accessible through the mainApp singleton.
    public:
        /// Application main window and GUI root class.
        MainWindow* m_mainWindow;

    private:
        /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Viewer* m_viewer;

        int m_frameCountBeforeUpdate;
        std::vector<Ra::FrameTimerData> m_timerData;

        QString m_fileToLoad;
    };

    /// This class contains the main application logic. It owns the engine and the GUI.
    class QRadiumApplication : public Ra::Gui::Application
    {
        RA_SINGLETON_INTERFACE(QRadiumApplication);

        // Enable simple two-ways interactions
        friend class MainApplication;
        friend class MainWindow;

    public:

        QRadiumApplication(int argc, char** argv);
        virtual ~QRadiumApplication();

        void loadFile(const std::string& path);

    protected:
        virtual void initializeInternal() override;
        virtual void updateUi(Ra::Plugins::RadiumPluginInterface* plugin) override;

        virtual void frameStart() override;
        virtual void handleEvents() override;
        virtual void handlePicking() override;
        virtual void render() override;
        virtual void frameEnd() override;

        virtual void runStart() override {}
        virtual void runEnd() override {}

        virtual const Ra::Engine::Renderer* getRenderer() const override { return m_viewer->getRenderer(); }

    private:
        void addBasicShaders();
        void setupScene();

    public:
        /// Application main window and GUI root class.
        MainWindow* m_mainWindow;

    private:
        /// Main application handler.
        MainApplication* m_mainApplication;

        /// Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Viewer* m_viewer;

        int m_frameCountBeforeUpdate;
        std::vector<Ra::FrameTimerData> m_timerData;

        std::string m_fileToLoad;
    };
}

#ifdef mainApp
#undef mainApp
#endif
#define mainApp QRadium::QRadiumApplication::getInstance()
