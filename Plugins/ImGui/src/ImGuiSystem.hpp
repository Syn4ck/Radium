#ifndef IMGUIPLUGIN_SYSTEM_HPP_
#define IMGUIPLUGIN_SYSTEM_HPP_

#include <Engine/System/System.hpp>
#include <Engine/RadiumEngine.hpp>

#include <ImGuiPlugin.hpp>

namespace ImGuiPlugin
{

    /**
     *
     * \class ImGuiSystem
     *
     * @brief The ImGuiSystem class
     *
     */

    class IMGUI_PLUGIN_API ImGuiSystem :  public Ra::Engine::System
    {
    public:

        /// Create a new ImGui system.
        ImGuiSystem();

        /// Task generator that asks <b>no</b> new thread
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo
                                  ) override;

    };

}


#endif // IMGUIPLUGIN_SYSTEM_HPP_
