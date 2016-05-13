#include <ImGuiComponent.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <iostream>

using Ra::Engine::ComponentMessenger;

namespace ImGuiPlugin
{

    // FIXME(hugo) these functions are empty but this will change

/*
    bool ImGuiComponent::picked(uint drawableIdx)
    {
        // ...
        return false;
    }

    void ImGuiComponent::getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const
    {
        // ...
    }

    void ImGuiComponent::setProperty(const Ra::Engine::EditableProperty &prop)
    {
        // ...
    }
*/

    void ImGuiComponent::initialize()
    {
        std::cout << "<* COMPONENT INIT *>" << std::endl;
        // ...
    }

    void ImGuiComponent::update(Scalar dt)
    {
        std::cout << "<* COMPONENT UPDATE *>" << std::endl;
        // ...
    }

    void ImGuiComponent::setupIO(const std::string &id)
    {
        std::cout << "<* COMPONENT IO SETUP *>" << std::endl;
        // ...
    }

} // namespace ImGuiPlugin
