#ifndef IMGUIPLUGIN_RO_HPP_
#define IMGUIPLUGIN_RO_HPP_

#include <ImGuiPlugin.hpp>
#include <ImGuiGraphViewer.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace ImGuiPlugin
{

    /**
     * \class ImGuiRenderObject
     *
     * @brief The ImGuiRenderObject class exports a render object with its
     * own render() method to render ImGui interface.
     *
     * blabla
     *
     */

    class IMGUI_PLUGIN_API ImGuiRenderObject : public Ra::Engine::RenderObject
    {
    public:
        ImGuiRenderObject(const std::string& name, ImGuiComponent* comp,
                          Ra::Core::MultiGraph<Ra::Engine::Pass>& passgraph,
                          const Ra::Engine::RenderObjectType& type, int lifetime = -1);

        virtual ~ImGuiRenderObject() {}

        /// Initialize the viewer with the graph.
        /// @warning The graph must have been fetched prior to this.
        void init();

        /// Override method render of RenderObject to get a custom behavior at rendering.
        virtual void render(const Ra::Engine::RenderParameters& lightParams,
                            Ra::Core::Matrix4 viewMatrix,
                            Ra::Core::Matrix4 projMatrix,
                            const Ra::Engine::ShaderProgram* altShader = nullptr) override;

    public:
        /// Parent component for the callbacks.
        ImGuiComponent* m_imguicomp;

        /// Node graph
        Ra::Core::MultiGraph<Ra::Engine::Pass>& m_passgraph;

        /// Node viewer
        ImGui::GraphViewer<Ra::Engine::Pass>    m_viewgraph;

        ///{@
        /// Display status of ImGui windows
        bool m_displayPassGraph;
        bool m_demoUI;
        ///@}

        Ra::Engine::RadiumEngine* m_engine;
    };

} // namespace ImGuiPlugin

#endif // IMGUIPLUGIN_RO_H
