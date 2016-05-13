#include <ImGuiComponent.hpp>


#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectTypes.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <iostream>

namespace ImGuiPlugin
{

    /// @brief instantiate the RenderObject on which we will draw imgui
    void ImGuiComponent::initialize()
    {
        std::cout << "<* COMPONENT INIT *>" << std::endl;
        // ...

        // get RenderObjectManager
        Ra::Engine::RenderObjectManager* roMgr = getRoMgr();


//        Ra::Core::TriangleMesh mesh = Ra::Core::MeshUtils::makeZNormalQuad(Ra::Core::Vector2( -1.f, 1.f));

//        m_quadMesh.reset( new Ra::Core::Mesh( "quad" ) );
//        m_quadMesh->loadGeometry( mesh );
//        m_quadMesh->updateGL();

        // create new RenderObject of type UI to render imgui component

//        |>|>|> THIS IS DA THING TO DO RIGHT NOW <|<|<|

//        Ra::Engine::RenderObject* ro =
//                Ra::Engine::RenderObject::createRenderObject("RO_imgui_" + m_contentName, this,
//                                                             Ra::Engine::RenderObjectType::UI,
//                                                             m_quadMesh, ...);

        // add it to the RenderObjectManager
//        roMgr->addRenderObject(ro);
    }

    /// @brief update component data
    /// @note actual use ?
    void ImGuiComponent::update(Scalar dt)
    {
        std::cout << "<* COMPONENT UPDATE *>" << std::endl;
        // ...
    }

    //void ImGuiComponent::setupIO(const std::string &id)
    //{
        //std::cout << "<* COMPONENT IO SETUP *>" << std::endl;
        // ...
    //}

} // namespace ImGuiPlugin
