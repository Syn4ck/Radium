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

    class IMGUI_PLUGIN_API ImGuiComponent : public Ra::Engine::Component
    {
    public:
        /// @brief handle the initialization process of ImGui
        ImGuiComponent(const std::string& name) : Component(name), m_contentName(name)
        {
        }

        /// @brief handle the destruction of ImGui
        virtual ~ImGuiComponent()
        {
            ImGuiGL3::shutdown();
        }

        /// @brief instantiate the RenderObject on which we will draw imgui
        virtual void initialize() override;

        /// @brief get the renderer's data from the engine
        void setupIO( const std::string& id );

        /// @brief State of displaying pass editor
        void setPassesEditor( bool state );

        /// @brief State of displaying demo code
        void setDemoUI(bool state);

        //Ra::Core::MultiGraph<Ra::Engine::Pass>& getPassGraph();
        void setWidthIn( const uint* w );
        void setHeightIn( const uint* h );

        void updateSize(const std::string& id);

    public:
        std::shared_ptr<Ra::Engine::Mesh> m_quadMesh;

        // renderer data
        uint m_width;
        uint m_height;

        Ra::Engine::RadiumEngine* m_engine;

    private:
        std::string m_contentName;
        std::unique_ptr<ImGuiRenderObject> m_ro;

    };

} // namespace ImGuiPlugin

#endif //IMGUIPLUGIN_COMPONENT_HPP_
