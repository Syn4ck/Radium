#include <ImGuiRenderObject.hpp>

#include <imgui.h>
#include <ImGuiGl3.hpp>
#include <ImGuiNodeGraph.hpp>
#include <ImGuiComponent.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <iostream>

namespace ImGuiPlugin {

    ImGuiRenderObject::ImGuiRenderObject(const std::string& name, ImGuiComponent* comp,
                                         Ra::Core::MultiGraph<Ra::Engine::Pass>& passgraph,
                                         const Ra::Engine::RenderObjectType& type, int lifetime)
        : Ra::Engine::RenderObject(name, (Ra::Engine::Component*)comp, type, lifetime)
        , m_imguicomp( comp )
        , m_passgraph( passgraph )
        , m_viewgraph( &passgraph )
        , m_displayPassGraph( false )
        , m_demoUI( true )
    {
    }

    void ImGuiRenderObject::render(const Ra::Engine::RenderParameters& lightParams, Ra::Core::Matrix4 viewMatrix,
                                   Ra::Core::Matrix4 projMatrix, const Ra::Engine::ShaderProgram* altShader)
    {
        if (! isVisible())
        {
            return;
        }

        m_imguicomp->updateSize("HUD");
        ImGuiGL3::newFrame(m_engine, m_imguicomp->m_width, m_imguicomp->m_height);

        bool isNodeOpened = true;

        if (m_demoUI)
        {
            // test window
            ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&isNodeOpened);
        }

        if (m_displayPassGraph)
        {
            // pass graph window
            m_viewgraph.init();
            m_viewgraph.Begin(&isNodeOpened);
            m_viewgraph.End();
        }

        ImGui::Render();
    }

} // namespace ImGuiPlugin
