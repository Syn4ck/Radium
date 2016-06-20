#ifndef IMGUIPLUGIN_COMPONENT_HPP_
#define IMGUIPLUGIN_COMPONENT_HPP_

#include <ImGuiGl3.hpp>
#include <ImGuiPlugin.hpp>
#include <ImGuiGraphViewer.hpp>
#include <ImGuiRenderObject.hpp>

#include <Engine/RaEngine.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Passes/Passes.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

namespace ImGuiPlugin
{

    /**
     *
     * \class ImGuiComponent
     *
     * @brief The ImGuiComponent class communicates with others plugins or
     * with the Radium.
     *
     * Will try to get width, height and post-process graph from the renderer.
     *
     */


    class IMGUI_PLUGIN_API ImGuiComponent : public Ra::Engine::Component
    {
    public:
        /// Handle the initialization process of ImGui.
        ImGuiComponent(const std::string& name) : Component(name), m_contentName(name)
        {
        }

        /// Handle the destruction of ImGui.
        virtual ~ImGuiComponent()
        {
            ImGuiGL3::shutdown();
        }

        /// Instantiate the RenderObject on which we will draw imgui.
        virtual void initialize() override;

        /// Get the renderer's data from the engine.
        void setupIO( const std::string& id );

        /// Display or not pass editor.
        void setPassesEditor( bool state );

        /// Display or not demo code.
        void setDemoUI(bool state);

        ///{@
        /// Functions used by the ComponentMessenger.
        void setWidthIn( const uint* w );
        void setHeightIn( const uint* h );
        ///@}

        /// Fetch width and height from the Renderer
        void updateSize(const std::string& id);

    public:
        std::shared_ptr<Ra::Engine::Mesh> m_quadMesh;

        ///{@
        /// Renderer data
        uint m_width;
        uint m_height;
        ///@}

        Ra::Engine::RadiumEngine* m_engine;

    private:
        /// Prefix to fetch data
        std::string m_contentName;

        /// ImGuiRenderObject
        std::unique_ptr<ImGuiRenderObject> m_ro;
    };

} // namespace ImGuiPlugin

#endif //IMGUIPLUGIN_COMPONENT_HPP_
