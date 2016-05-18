#include <ImGuiSystem.hpp>
#include <ImGuiComponent.hpp>

//#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

namespace ImGuiPlugin
{

    ImGuiSystem::ImGuiSystem() : m_displayPassGraph( false ) {}

    void ImGuiSystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // no tasks in there, this plugin only create an ui renderobject
    }

    void ImGuiSystem::setPassesEditor( bool state )
    {
        m_displayPassGraph = state;
    }

} // namespace ImGuiPlugin
