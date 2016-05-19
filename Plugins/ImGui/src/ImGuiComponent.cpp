#include <ImGuiComponent.hpp>

#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Core/Mesh/MeshPrimitives.hpp>

#include <iostream>

namespace ImGuiPlugin
{

    void ImGuiComponent::initialize()
    {
        // create new RenderObject of type ImGuiRO to render imgui component
        m_ro.reset( new ImGuiRenderObject("RO_imgui_" + m_contentName, this, Ra::Engine::RenderObjectType::UI) );

        // setup IO to import needed informations
        setupIO("HUD");

        fetch("HUD");

        // add the new RO to ui component
        Ra::Engine::UiComponent* comp = Ra::Engine::SystemEntity::uiCmp();
        comp->addRenderObject(m_ro.get());
    }

    void ImGuiComponent::setupIO(const std::string &id)
    {
        using Ra::Engine::ComponentMessenger;
        using Ra::Core::MultiGraph;
        using Ra::Engine::Pass;

        /* register to:
         * - multigraph
         * - width
         * - height
         */

        //ComponentMessenger::CallbackTypes<MultiGraph<Pass>>::ReadWrite passRw = std::bind( &ImGuiComponent::getPassGraphRw, this );
        //ComponentMessenger::getInstance()->registerReadWrite<MultiGraph<Pass>>( nullptr, this, id + "_passgraph", passRw );

        ComponentMessenger::CallbackTypes<uint>::Setter wIn = std::bind( &ImGuiComponent::setWidthIn, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<uint>( nullptr, this, id + "_width", wIn );

        ComponentMessenger::CallbackTypes<uint>::Setter hIn = std::bind( &ImGuiComponent::setHeightIn, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<uint>( nullptr, this, id + "_height", hIn );
    }

    void ImGuiComponent::fetch(const std::string& id)
    {
        bool canW, canH, canP;

        using Ra::Engine::ComponentMessenger;
        using Ra::Core::MultiGraph;
        using Ra::Engine::Pass;

        if ((canW = ComponentMessenger::getInstance()->canGet<uint>(nullptr, id + "_width")))
        {
            m_width = ComponentMessenger::getInstance()->get<uint>(nullptr, id + "_width");
        }

        if ((canH = ComponentMessenger::getInstance()->canGet<uint>(nullptr, id + "_height")))
        {
            m_height = ComponentMessenger::getInstance()->get<uint>(nullptr, id + "_height");
        }

        if ((canP = ComponentMessenger::getInstance()->canRw<MultiGraph<Pass>>(nullptr, id + "_passgraph")))
        {
//            m_passGraph = &ComponentMessenger::getInstance()->rw<MultiGraph<Pass>>(nullptr, id + "_passgraph");
        }

        std::cout << "------------\nreceived:\n------------" << std::endl;
        std::cout << "width " << m_width << std::endl;
        std::cout << "height " << m_height << std::endl;
        std::cout << "passgraph " << (void*) &m_passGraph << std::endl;
    }

    void ImGuiComponent::setPassesEditor( bool state )
    {
        m_ro->m_displayPassGraph = state;
    }

    Ra::Core::MultiGraph<Ra::Engine::Pass>* ImGuiComponent::getPassGraphRw()
    {
        return m_passGraph;
    }

    void ImGuiComponent::setWidthIn(const uint* w)
    {
        m_width = *w;
    }

    void ImGuiComponent::setHeightIn(const uint* h)
    {
        m_height = *h;
    }

} // namespace ImGuiPlugin
