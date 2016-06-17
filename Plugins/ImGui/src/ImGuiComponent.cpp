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
        // fetch pass graph
        Ra::Core::MultiGraph<Ra::Engine::Pass>& pgraph = Ra::Engine::ComponentMessenger::getInstance()
                ->rw<Ra::Core::MultiGraph<Ra::Engine::Pass>>(nullptr, "HUD_passgraph");

        // create new RenderObject of type ImGuiRO to render imgui component
        m_ro.reset( new ImGuiRenderObject("RO_imgui_" + m_contentName, this, pgraph,  Ra::Engine::RenderObjectType::UI) );
        m_ro->m_engine = m_engine;
        m_ro->init();

        // setup IO to import needed informations
        setupIO("HUD");

        // add the new RO to ui component
        Ra::Engine::UiComponent* comp = Ra::Engine::SystemEntity::uiCmp();
        comp->addRenderObject(m_ro.get());

        // init the ImGui GL part
        ImGuiGL3::init(m_engine);
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
         * - ...
         */

        ComponentMessenger::CallbackTypes<uint>::Setter wIn = std::bind( &ImGuiComponent::setWidthIn, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<uint>( nullptr, this, id + "_width", wIn );

        ComponentMessenger::CallbackTypes<uint>::Setter hIn = std::bind( &ImGuiComponent::setHeightIn, this, std::placeholders::_1 );
        ComponentMessenger::getInstance()->registerInput<uint>( nullptr, this, id + "_height", hIn );
    }

    void ImGuiComponent::updateSize(const std::string& id)
    {
        bool canW, canH;

        using Ra::Engine::ComponentMessenger;
        using Ra::Core::MultiGraph;

        if ((canW = ComponentMessenger::getInstance()->canGet<uint>(nullptr, id + "_width")))
        {
            m_width = ComponentMessenger::getInstance()->get<uint>(nullptr, id + "_width");
        }

        if ((canH = ComponentMessenger::getInstance()->canGet<uint>(nullptr, id + "_height")))
        {
            m_height = ComponentMessenger::getInstance()->get<uint>(nullptr, id + "_height");
        }
    }

    void ImGuiComponent::setPassesEditor( bool state )
    {
        m_ro->m_displayPassGraph = state;
    }

    void ImGuiComponent::setDemoUI( bool state )
    {
        m_ro->m_demoUI = state;
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
