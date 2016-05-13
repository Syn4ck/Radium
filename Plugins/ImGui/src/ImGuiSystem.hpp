#ifndef IMGUIPLUGIN_SYSTEM_HPP_
#define IMGUIPLUGIN_SYSTEM_HPP_

#include <Engine/System/System.hpp>

#include <ImGuiPlugin.hpp>

namespace ImGuiPlugin
{
    class IMGUI_PLUGIN_API ImGuiSystem :  public Ra::Engine::System
    {
    public:

        /// @brief Create a new im-gui system
        ImGuiSystem();

        /// @brief Task generator that generate nothing
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        /// @brief State of displaying pass editor
        void setPassesEditor( bool state );

    private:
        bool m_displayPassGraph; /// True when displaying the pass editor graph
    };
}


#endif // IMGUIPLUGIN_SYSTEM_HPP_
