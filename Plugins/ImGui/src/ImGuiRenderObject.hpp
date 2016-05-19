#ifndef IMGUIPLUGIN_RO_HPP_
#define IMGUIPLUGIN_RO_HPP_

#include <ImGuiPlugin.hpp>
#include <ImGuiNodeGraph.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace ImGuiPlugin
{

    class IMGUI_PLUGIN_API ImGuiRenderObject : public Ra::Engine::RenderObject
    {
    public:
        ImGuiRenderObject(const std::string& name, ImGuiComponent* comp,
                          const Ra::Engine::RenderObjectType& type, int lifetime = -1);

        virtual ~ImGuiRenderObject() {}

        /// @brief override method render of renderObject to get a custom behavior at rendering
        virtual void render(const Ra::Engine::RenderParameters& lightParams,
                            Ra::Core::Matrix4 viewMatrix,
                            Ra::Core::Matrix4 projMatrix,
                            const Ra::Engine::ShaderProgram* altShader = nullptr) override;

    public:
        // component for the callbacks
        const ImGuiComponent* m_imguicomp;

        // node viewer
        ImGui::GraphViewer<Ra::Engine::Pass> m_passViewer;
        bool m_displayPassGraph;
    };

} // namespace ImGuiPlugin

#endif // IMGUIPLUGIN_RO_H
