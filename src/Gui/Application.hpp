#ifndef RADIUMENGINE_GUIBASE_APPLICATION_HPP
#define RADIUMENGINE_GUIBASE_APPLICATION_HPP

#include <vector>
#include <memory>

#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Gui/TimerData/FrameTimerData.hpp>

namespace Ra
{
    namespace Plugins
    {
        class RadiumPluginInterface;
    }
}

namespace Ra
{
    namespace Guibase
    {
        class Application
        {
        public:
            Application(int argc, char** argv);
            virtual ~Application();

            void run();

            const Engine::RadiumEngine* getEngine () const { return m_engine.get();}

        protected:
            virtual void initializeInternal() = 0;
            virtual void updateUi(Plugins::RadiumPluginInterface* plugin) = 0;

            virtual void frameStart() = 0;
            virtual void handleEvents() = 0;
            virtual void handlePicking() = 0;
            virtual void render() = 0;
            virtual void frameEnd() = 0;

            virtual void runStart() = 0;
            virtual void runEnd() = 0;

            virtual const Engine::Renderer* getRenderer() const = 0;

        private:
            void frame();
            bool loadPlugins(const std::string& pluginsPath);

        public:
            std::unique_ptr<Engine::RadiumEngine> m_engine;
            std::unique_ptr<Core::TaskQueue> m_taskQueue;

            uint m_targetFPS;
            Scalar m_targetFrameTime;

        protected:
            int m_argc;
            char** m_argv;

            Scalar m_dt;

            Core::Timer::TimePoint m_lastFrameStart;
            FrameTimerData m_frameData;
            uint m_frameCounter;

            std::string m_pluginsPath;

            bool m_running;
        };
    }
}

#endif // RADIUMENGINE_GUIBASE_APPLICATION_HPP
