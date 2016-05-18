#include <ImGuiComponent.hpp>
#include <ImGuiRenderObject.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

#include <Core/Mesh/MeshPrimitives.hpp>

namespace ImGuiPlugin
{

    /// @brief instantiate the RenderObject on which we will draw imgui
    void ImGuiComponent::initialize()
    {
        // create mesh to render on
        Ra::Core::TriangleMesh mesh = Ra::Core::MeshUtils::makeZNormalQuad(Ra::Core::Vector2( -1.f, 1.f ));

        // and assign quadmesh to it
        m_quadMesh.reset( new Ra::Engine::Mesh( "quad" ) );
        m_quadMesh->loadGeometry( mesh );
        m_quadMesh->updateGL();

        // create new RenderObject of type ImGuiRO to render imgui component
        ImGuiRenderObject* ro = new ImGuiRenderObject("RO_imgui_" + m_contentName, this, Ra::Engine::RenderObjectType::UI);

        // get RenderObjectManager from UI component
        Ra::Engine::UiComponent* comp = Ra::Engine::SystemEntity::uiCmp();
        comp->addRenderObject(ro);

        // initialise the graph viewer from ImGui
        //m_passViewer(Ra::Engine);
    }

    /// @brief update component data
    /// @note actual use ?
    void ImGuiComponent::update(Scalar dt)
    {
        // ...
        //std::cout << "<* COMPONENT UPDATE *>" << std::endl;
    }

    //void ImGuiComponent::setupIO(const std::string &id)
    //{
        // ...
        //std::cout << "<* COMPONENT IO SETUP *>" << std::endl;
    //}

} // namespace ImGuiPlugin
