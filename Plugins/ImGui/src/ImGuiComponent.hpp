#ifndef IMGUIPLUGIN_COMPONENT_HPP_
#define IMGUIPLUGIN_COMPONENT_HPP_

#include <ImGuiGl3.hpp>
#include <ImGuiPlugin.hpp>
#include <ImGuiNodeGraph.hpp>

#include <Engine/RaEngine.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Passes/Passes.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

namespace ImGuiPlugin
{

    class IMGUI_PLUGIN_API ImGuiComponent : public Ra::Engine::Component
    {
    public:
        ImGuiComponent(const std::string& name) : Component(name), m_contentName(name)
        {
            initialize();
            ImGuiGL3::init();
        }

        virtual ~ImGuiComponent()
        {
            ImGuiGL3::shutdown();
        }

        virtual void initialize() override;

        void update(Scalar dt);

    private:
        // Component communication
        // void setupIO( const std::string& id );

    public:
        std::shared_ptr<Ra::Engine::Mesh> m_quadMesh;

    private:
        std::string m_contentName;
        ImGui::GraphViewer<Ra::Engine::Pass> m_passViewer;

    };

} // namespace ImGuiPlugin

#endif //IMGUIPLUGIN_COMPONENT_HPP_
