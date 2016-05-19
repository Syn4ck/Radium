#include <ImGuiRenderObject.hpp>

#include <imgui.h>
#include <ImGuiGl3.hpp>
#include <ImGuiNodeGraph.hpp>
#include <ImGuiComponent.hpp>

#include <iostream>

namespace ImGuiPlugin {

    ImGuiRenderObject::ImGuiRenderObject(const std::string& name, ImGuiComponent* comp,
                      const Ra::Engine::RenderObjectType& type, int lifetime)
        : Ra::Engine::RenderObject(name, (Ra::Engine::Component*)comp, type, lifetime)
        , m_imguicomp( comp )
        , m_displayPassGraph( true )
    {
    }

    void ImGuiRenderObject::render(const Ra::Engine::RenderParameters& lightParams, Ra::Core::Matrix4 viewMatrix,
                                   Ra::Core::Matrix4 projMatrix, const Ra::Engine::ShaderProgram* altShader)
    {
        if (! isVisible())
        {
            return;
        }

        ImGuiGL3::newFrame(m_imguicomp->m_width, m_imguicomp->m_height);

        if (m_displayPassGraph)
        {
            bool isNodeOpened = true;

            // test window
            ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&isNodeOpened);

            // pass graph window
            //m_graphview.Begin(&isNodeOpened);
            //m_graphview.End();
        }

        ImGui::Render();

        // render the mesh
        //getMesh()->render();
    }

} // namespace ImGuiPlugin
