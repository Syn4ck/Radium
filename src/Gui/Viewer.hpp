#ifndef RADIUMENGINE_GUIBASE_VIEWER_HPP
#define RADIUMENGINE_GUIBASE_VIEWER_HPP

#include <Gui/RaGuiBase.hpp>

#if defined(OS_WINDOWS)
#include <Engine/Renderer/OpenGL/glew.h>
#endif

#include <memory>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Event/EventEnums.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>

#include <Gui/Gizmo/GizmoManager.hpp>
#include <Gui/Camera/CameraInterface.hpp>

namespace Ra
{
    namespace Gui
    {
        /// The whole purpose of the viewer is to call Engine::Renderer stuff and forward
        /// events to cameras or gizmos
        class RA_GUIBASE_API Viewer
        {
        public:
            Viewer();
            ~Viewer();

            void initialize(uint width, uint height);
            void render(const Scalar dt);
            void resize(uint width, uint height);

            const Engine::Renderer* getRenderer() const { return m_renderer.get(); }

            void addCamera(const std::shared_ptr<Ra::Gui::CameraInterface>& camera) { m_cameras.push_back(camera); }
            const CameraInterface* getCurrentCamera() const { return m_currentCamera; }
            CameraInterface* getCurrentCamera() { return m_currentCamera; }
            uint getCameraCount() const { return m_cameras.size(); }
            void setCurrentCamera(uint index)
            {
                CORE_ASSERT(m_cameras.size() > index, "Wrong camera index");
                m_currentCamera = m_cameras[index].get();
            }

            const GizmoManager* getGizmoManager() const { return m_gizmoManager; }
            GizmoManager* getGizmoManager() { return m_gizmoManager; }

            void enablePostProcess(bool enabled) { m_renderer->enablePostProcess(enabled); }
            void reloadShaders() { m_renderer->reloadShaders(); }
            void displayTexture(const std::string& tex) { m_renderer->displayTexture(tex); }

            // FIXME(charly): Remove this, and properly handle scenes
            void handleFileLoading(const std::string& file) {  m_renderer->handleFileLoading(file); }

            void processPicking(std::vector<int>& leftClickQueries, std::vector<int>& middleClickQueries, std::vector<int>& rightClickQueries);

            void handleEvents();

        private:
            uint m_width, m_height;

            // Use only one renderer for now.
            std::shared_ptr<Engine::Renderer> m_renderer;

            std::vector<std::shared_ptr<CameraInterface>> m_cameras;
            CameraInterface* m_currentCamera;

            GizmoManager* m_gizmoManager;
        };
    }
}

#endif // RADIUMENGINE_GUIBASE_VIEWER_HPP
